#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <stdio.h>
#include <arpa/inet.h>

#define ENABLE_SEND		1
#define ENABLE_ARP		1

#define NUM_MBUFS (4096-1)
#define BURST_SIZE	32

#if ENABLE_SEND
#define MAKE_IPV4_ADDR(a, b, c, d) (a + (b<<8) + (c<<16) + (d<<24))
static uint32_t gLocalIp = MAKE_IPV4_ADDR(192, 168, 0, 108);
static uint32_t gSrcIp;
static uint32_t gDstIp;
static uint8_t gSrcMac[RTE_ETHER_ADDR_LEN];
static uint8_t gDstMac[RTE_ETHER_ADDR_LEN];
static uint16_t gSrcPort;
static uint16_t gDstPort;
#endif

int gDpdkPortId = 0;

static const struct rte_eth_conf port_conf_default = {
    .rxmode = {.max_rx_pkt_len = RTE_ETHER_MAX_LEN }
};

static void ng_init_port(struct rte_mempool *mbuf_pool) {
    uint16_t nb_sys_ports= rte_eth_dev_count_avail();
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Supported eth found\n");
    }
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(gDpdkPortId, &dev_info);
    const int num_rx_queues = 1;
    const int num_tx_queues = 1;
    struct rte_eth_conf port_conf = port_conf_default;
    rte_eth_dev_configure(gDpdkPortId, num_rx_queues, num_tx_queues, &port_conf);
    if (rte_eth_rx_queue_setup(gDpdkPortId, 0 , 1024, 
                rte_eth_dev_socket_id(gDpdkPortId),NULL, mbuf_pool) < 0) {
        rte_exit(EXIT_FAILURE, "Could not setup RX queue\n");
    }
#if ENABLE_SEND
    struct rte_eth_txconf txq_conf = dev_info.default_txconf;                               // 发送队列（TX）的默认最佳配置: 发送描述符模式 卸载功能（checksum, TSO 等） 阈值、预取、回写机制
    txq_conf.offloads = port_conf.rxmode.offloads;
    if (rte_eth_tx_queue_setup(gDpdkPortId, 0 , 1024, 
                rte_eth_dev_socket_id(gDpdkPortId), &txq_conf) < 0) {
        rte_exit(EXIT_FAILURE, "Could not setup TX queue\n");
    }
#endif
    if (rte_eth_dev_start(gDpdkPortId) < 0 ) {
        rte_exit(EXIT_FAILURE, "Could not start\n");
    }
    rte_eth_promiscuous_enable(gDpdkPortId);
}

static int ng_encode_udp_pkt(uint8_t *msg, unsigned char *data, uint16_t total_len) {
    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;                                // 1 ethhdr
    rte_memcpy(eth->s_addr.addr_bytes, gSrcMac, RTE_ETHER_ADDR_LEN);
    rte_memcpy(eth->d_addr.addr_bytes, gDstMac, RTE_ETHER_ADDR_LEN);
    eth->ether_type = htons(RTE_ETHER_TYPE_IPV4);
    struct rte_ipv4_hdr *ip = (struct rte_ipv4_hdr *)(msg + sizeof(struct rte_ether_hdr));  // 2 iphdr 
    ip->version_ihl = 0x45;
    ip->type_of_service = 0;
    ip->total_length = htons(total_len - sizeof(struct rte_ether_hdr));
    ip->packet_id = 0;
    ip->fragment_offset = 0;
    ip->time_to_live = 64;
    ip->next_proto_id = IPPROTO_UDP;
    ip->src_addr = gSrcIp;
    ip->dst_addr = gDstIp;
    ip->hdr_checksum = rte_ipv4_cksum(ip);
    struct rte_udp_hdr *udp = (struct rte_udp_hdr *)(msg +                                  // 3 udphdr 
            sizeof(struct rte_ether_hdr) + sizeof(struct rte_ipv4_hdr));
    udp->src_port = gSrcPort;
    udp->dst_port = gDstPort;
    uint16_t udplen = total_len - sizeof(struct rte_ether_hdr) - sizeof(struct rte_ipv4_hdr);
    udp->dgram_len = htons(udplen);
    rte_memcpy((uint8_t*)(udp+1), data, udplen);
    udp->dgram_cksum = rte_ipv4_udptcp_cksum(ip, udp);
    struct in_addr addr;
    addr.s_addr = gSrcIp;
    printf(" --> src: %s:%d, ", inet_ntoa(addr), ntohs(gSrcPort));
    addr.s_addr = gDstIp;
    printf("dst: %s:%d\n", inet_ntoa(addr), ntohs(gDstPort));
    return 0;
}


static struct rte_mbuf * ng_send_udp(struct rte_mempool *mbuf_pool, uint8_t *data, uint16_t length) {
    const unsigned total_len = length + 42;
    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
    if (!mbuf) {
        rte_exit(EXIT_FAILURE, "rte_pktmbuf_alloc\n");
    }
    mbuf->pkt_len = total_len;
    mbuf->data_len = total_len;
    uint8_t *pktdata = rte_pktmbuf_mtod(mbuf, uint8_t*);
    ng_encode_udp_pkt(pktdata, data, total_len);
    return mbuf;
}

#if ENABLE_ARP
static int ng_encode_arp_pkt(uint8_t *msg, uint8_t *dst_mac, uint32_t sip, uint32_t dip) {
    struct rte_ether_hdr *eth = (struct rte_ether_hdr *)msg;            // ethhdr
    rte_memcpy(eth->s_addr.addr_bytes, gSrcMac, RTE_ETHER_ADDR_LEN);
    rte_memcpy(eth->d_addr.addr_bytes, dst_mac, RTE_ETHER_ADDR_LEN);
    eth->ether_type = htons(RTE_ETHER_TYPE_ARP);
    struct rte_arp_hdr *arp = (struct rte_arp_hdr *)(eth + 1);          // arp 
    arp->arp_hardware = htons(1);                                       // 1: 以太网
    arp->arp_protocol = htons(RTE_ETHER_TYPE_IPV4);                     // 要解析的是 IPv4 地址
    arp->arp_hlen = RTE_ETHER_ADDR_LEN;                                 // 硬件地址长度：6字节MAC
    arp->arp_plen = sizeof(uint32_t);                                   // 协议地址长度：4字节IP
    arp->arp_opcode = htons(2);                                         //  2 = ARP 响应
    rte_memcpy(arp->arp_data.arp_sha.addr_bytes, gSrcMac, RTE_ETHER_ADDR_LEN);  // ARP Sender Hardware Address
    rte_memcpy( arp->arp_data.arp_tha.addr_bytes, dst_mac, RTE_ETHER_ADDR_LEN); // ARP Target Hardware Address
    arp->arp_data.arp_sip = sip;
    arp->arp_data.arp_tip = dip;
    return 0;
}

static struct rte_mbuf *ng_send_arp(struct rte_mempool *mbuf_pool, uint8_t *dst_mac, uint32_t sip, uint32_t dip) {
    const unsigned total_length = sizeof(struct rte_ether_hdr) + sizeof(struct rte_arp_hdr);
    struct rte_mbuf *mbuf = rte_pktmbuf_alloc(mbuf_pool);
    if (!mbuf) {
        rte_exit(EXIT_FAILURE, "rte_pktmbuf_alloc\n");
    }
    mbuf->pkt_len = total_length;
    mbuf->data_len = total_length;
    uint8_t *pkt_data = rte_pktmbuf_mtod(mbuf, uint8_t *);
    ng_encode_arp_pkt(pkt_data, dst_mac, sip, dip);
    return mbuf;
}
#endif


int main(int argc, char *argv[]) {
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL init\n");
    }
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("mbuf pool", NUM_MBUFS,
            0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Could not create mbuf pool\n");
    }
    ng_init_port(mbuf_pool);
    rte_eth_macaddr_get(gDpdkPortId, (struct rte_ether_addr *)gSrcMac);
    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE];
        unsigned num_recvd = rte_eth_rx_burst(gDpdkPortId, 0, mbufs, BURST_SIZE);
        if (num_recvd > BURST_SIZE) {
            rte_exit(EXIT_FAILURE, "Error receiving from eth\n");
        }
        unsigned i = 0;
        for (i = 0; i < num_recvd; i++) {
            struct rte_ether_hdr *ehdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr*);
#if ENABLE_ARP
            if (ehdr->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP)) {
                struct rte_arp_hdr *ahdr = rte_pktmbuf_mtod_offset(mbufs[i], 
                        struct rte_arp_hdr *, sizeof(struct rte_ether_hdr));
                struct in_addr addr;
                addr.s_addr = ahdr->arp_data.arp_tip;
                printf("arp ---> src: %s ", inet_ntoa(addr));
                addr.s_addr = gLocalIp;
                printf(" local: %s \n", inet_ntoa(addr));
                if (ahdr->arp_data.arp_tip == gLocalIp) {
                    struct rte_mbuf *arpbuf = ng_send_arp(mbuf_pool, ahdr->arp_data.arp_sha.addr_bytes, 
                            ahdr->arp_data.arp_tip, ahdr->arp_data.arp_sip);
                    rte_eth_tx_burst(gDpdkPortId, 0, &arpbuf, 1);
                    rte_pktmbuf_free(arpbuf);
                    rte_pktmbuf_free(mbufs[i]);
                }
                continue;
            }
#endif
            if (ehdr->ether_type != rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                continue;
            }
            struct rte_ipv4_hdr *iphdr =  rte_pktmbuf_mtod_offset(mbufs[i], struct rte_ipv4_hdr *, 
                    sizeof(struct rte_ether_hdr));
            if (iphdr->next_proto_id == IPPROTO_UDP) {
                struct rte_udp_hdr *udphdr = (struct rte_udp_hdr *)(iphdr + 1);
#if ENABLE_SEND
                rte_memcpy(gDstMac, ehdr->s_addr.addr_bytes, RTE_ETHER_ADDR_LEN);
                rte_memcpy(&gSrcIp, &iphdr->dst_addr, sizeof(uint32_t));
                rte_memcpy(&gDstIp, &iphdr->src_addr, sizeof(uint32_t));
                rte_memcpy(&gSrcPort, &udphdr->dst_port, sizeof(uint16_t));
                rte_memcpy(&gDstPort, &udphdr->src_port, sizeof(uint16_t));
#endif
                uint16_t length = ntohs(udphdr->dgram_len);
                *((char*)udphdr + length) = '\0';
                struct in_addr addr;
                addr.s_addr = iphdr->src_addr;
                printf("src: %s:%d, ", inet_ntoa(addr), ntohs(udphdr->src_port));
                addr.s_addr = iphdr->dst_addr;
                printf("dst: %s:%d\n", inet_ntoa(addr), ntohs(udphdr->dst_port));
#if ENABLE_SEND
                struct rte_mbuf *txbuf = ng_send_udp(mbuf_pool, (uint8_t *)(udphdr+1), length);
                rte_eth_tx_burst(gDpdkPortId, 0, &txbuf, 1);
                rte_pktmbuf_free(txbuf);
#endif
                rte_pktmbuf_free(mbufs[i]);
            }
        }
    }
    return 0;
}

