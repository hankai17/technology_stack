#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <stdio.h>
#include <arpa/inet.h>

#define NUM_MBUFS (4096-1)      // 内存池里的数据包数量（DPDK 建议 2^n -1）
#define BURST_SIZE	32          // 一次收包最多收 32 个（DPDK 批量收包）
int gDpdkPortId = 0;            // 绑定的 0 号网卡

static const struct rte_eth_conf port_conf_default = {
    .rxmode = {
        .max_rx_pkt_len = RTE_ETHER_MAX_LEN     // 最大以太网帧长度 1518 = MTU(1500) + ETH(14) + CRC(4)
    }
};

static void ng_init_port(struct rte_mempool *mbuf_pool) {
    uint16_t nb_sys_ports= rte_eth_dev_count_avail();           // 获取系统中被 DPDK 接管的网卡数量 // 命令行绑定的那个网卡
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Supported eth found\n");
    }
    struct rte_eth_dev_info dev_info;
    rte_eth_dev_info_get(gDpdkPortId, &dev_info);
    const int num_rx_queues = 1;
    const int num_tx_queues = 0;
    struct rte_eth_conf port_conf = port_conf_default;
    rte_eth_dev_configure(gDpdkPortId, num_rx_queues, num_tx_queues, &port_conf);

    struct rte_ether_addr my_mac;
    rte_eth_macaddr_get(gDpdkPortId, &my_mac);                                          // 读取当前网卡MAC
    printf("DPDK 网卡MAC: %02x:%02x:%02x:%02x:%02x:%02x\n",
           my_mac.addr_bytes[0], my_mac.addr_bytes[1], my_mac.addr_bytes[2],
           my_mac.addr_bytes[3], my_mac.addr_bytes[4], my_mac.addr_bytes[5]);

    if (rte_eth_rx_queue_setup(gDpdkPortId, 0 , 128,                                    // 初始化接收队列: 队列0, 接收队列(缓存)的大小128个包
                rte_eth_dev_socket_id(gDpdkPortId), NULL, mbuf_pool) < 0) {             // 给网卡 0 的接收队列 0 设置 128 个缓存格子，并且使用和网卡同一个 NUMA 节点的内存。
        rte_exit(EXIT_FAILURE, "Could not setup RX queue\n");
    }
    if (rte_eth_dev_start(gDpdkPortId) < 0 ) {                                          // 启动网卡
        rte_exit(EXIT_FAILURE, "Could not start\n");
    }
    rte_eth_promiscuous_enable(gDpdkPortId);
}


int main(int argc, char *argv[]) {
    if (rte_eal_init(argc, argv) < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL init\n");
    }
    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("mbuf pool", NUM_MBUFS,     // 创建 mbuf 内存池（存放收到的数据包）
            0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());                          // rte_socket_id: 获取当前代码运行在哪个 NUMA 节点（CPU 插槽）上 // dpdk的内存池 是绑定numa的
    if (mbuf_pool == NULL) {
        rte_exit(EXIT_FAILURE, "Could not create mbuf pool\n");
    }
    ng_init_port(mbuf_pool);
    while (1) {
        struct rte_mbuf *mbufs[BURST_SIZE];
        unsigned num_recvd = rte_eth_rx_burst(gDpdkPortId, 0, mbufs, BURST_SIZE);
        if (num_recvd > BURST_SIZE) {
            rte_exit(EXIT_FAILURE, "Error receiving from eth\n");
        }
        unsigned i = 0;
        for (i = 0;i < num_recvd;i ++) {
            struct rte_ether_hdr *ehdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr*);
            if (ehdr->ether_type != rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                continue;
            }
            printf("get packet\n");
            struct rte_ipv4_hdr *iphdr =  rte_pktmbuf_mtod_offset(mbufs[i], struct rte_ipv4_hdr *, 
                    sizeof(struct rte_ether_hdr));
            if (iphdr->next_proto_id == IPPROTO_UDP) {
                struct rte_udp_hdr *udphdr = (struct rte_udp_hdr *)(iphdr + 1);
                uint16_t length = ntohs(udphdr->dgram_len);
                *((char*)udphdr + length) = '\0';
                struct in_addr addr;
                addr.s_addr = iphdr->src_addr;
                printf("src: %s:%d, ", inet_ntoa(addr), udphdr->src_port);
                addr.s_addr = iphdr->dst_addr;
                //printf("dst: %s:%d, %s\n", inet_ntoa(addr), udphdr->src_port, (char *)(udphdr+1));
                printf("dst: %s:%d\n", inet_ntoa(addr), udphdr->src_port);
                rte_pktmbuf_free(mbufs[i]);
            }
        }
    }
    return 0;
}

