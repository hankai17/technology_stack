#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/version.h>
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/moduleparam.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/icmp.h>
#include <net/ip.h>
#include <net/route.h>
#include <asm/uaccess.h>
#include <linux/bitops.h>
#include <linux/capability.h>
#include <linux/cpu.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/hash.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mutex.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/socket.h>
#include <linux/sockios.h>
#include <linux/errno.h> 
#include <linux/interrupt.h> 
#include <linux/if_ether.h> 
#include <linux/netdevice.h> 
#include <linux/etherdevice.h> 
#include <linux/ethtool.h> 
#include <linux/notifier.h> 
#include <linux/skbuff.h> 
#include <net/net_namespace.h> 
#include <net/sock.h> 
#include <linux/rtnetlink.h> 
#include <linux/stat.h> 
#include <net/dst.h> 
#include <net/pkt_sched.h>
#include <net/checksum.h>
#include <linux/highmem.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/netpoll.h>
#include <linux/rcupdate.h>
#include <linux/delay.h>
#include <net/iw_handler.h>
#include <asm/current.h>
#include <linux/audit.h>
#include <linux/dmaengine.h>
#include <linux/err.h>
#include <linux/ctype.h>
#include <linux/if_arp.h>
#include <linux/if_vlan.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <net/arp.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/jhash.h>
#include <linux/random.h>
#include <linux/inet.h>
#include <trace/events/napi.h>
#include <trace/events/net.h>
#include <trace/events/skb.h>
#include <linux/pci.h>
#include <linux/inetdevice.h>
#include <linux/cpu_rmap.h>
#include <linux/static_key.h>
#include <linux/hashtable.h>
#include "tproxy_common.h"

#define DEBUG_OUTPUT 1

#ifdef DEBUG_OUTPUT
#define myprintk printk
#else
#define myprintk(args, ...)
#endif

/*static int do_output_filter = 1;*/

int filter_br_skb(struct sk_buff *skb)
{
    return skb->mark;
}

static inline const char *get_skb_desc(struct sk_buff *skb)
{
	switch (skb->mark) {
		case SESSION_TPROXY_OMARK_VXCLIENT:
			return "vxclient output skb";
		case SESSION_TPROXY_OMARK_VXSERVER:
			return "vxclient upstream output skb";
		case SESSION_TPROXY_OMARK_SWCLIENT:
			return "switch output skb";
		case SESSION_TPROXY_OMARK_SWSERVER:
			return "switch upstream output skb";
		default:
			return "unknown skb";
	}
}

static inline int tcp_output_send_skb(struct sk_buff *skb, struct net_device *dev, u16 type, unsigned int hh_len, const char *desc)
{
	struct ethhdr      *eth;
	int                 rc = 0;

#if 0
	unsigned char feth1[] = { 0x00, 0x50, 0x56, 0x23, 0x2a, 0xa8 };
	unsigned char feth0[] = { 0x00, 0x50, 0x56, 0x2e, 0x78, 0xd0 };

	/* fack address */
	unsigned char mac_client[] = { 0x00, 0x0c, 0x29, 0x02, 0xca, 0x83 };
	unsigned char mac_server[] = { 0x00, 0x0c, 0x29, 0x27, 0x54, 0x36 };
#else
	/* fack addresses */
	unsigned char feth1[] = { 0x00, 0x50, 0x56, 0x23, 0x2a, 0xa1 };
	unsigned char feth0[] = { 0x00, 0x50, 0x56, 0x2e, 0x78, 0xd1 };

	unsigned char mac_client[] = { 0x00, 0x0c, 0x29, 0x02, 0xca, 0x88 };
	unsigned char mac_server[] = { 0x00, 0x0c, 0x29, 0x27, 0x54, 0x34 };
#endif


	/* XXX IPV6 protocol not set here */
	myprintk("[%s] skb->protocol = 0x%04X (%s) ======================headerlen =%d \n",
			desc, type, type == ETH_P_IP ? "ETH_P_IP" : "ETH_P_IPV6", hh_len);

	if (unlikely(skb_headroom(skb) < hh_len)) {
		struct sk_buff *skb2;

		skb2 = skb_realloc_headroom(skb, hh_len);
		if (skb2 == NULL) {
			kfree_skb(skb);
			return NF_STOLEN;
		}

		if (skb->sk)
			skb_set_owner_w(skb2, skb->sk);
		consume_skb(skb);
		skb = skb2;
	}

	eth = eth_hdr(skb);
	myprintk("[%s] eth=%p= skb->data =%p, skb->head = %p=================================\n", desc, eth, skb->data, skb->head);

	/*skb_push(skb, hh_len);*/
	skb_push(skb, 14);
	skb_reset_mac_header(skb);

	eth = eth_hdr(skb);
	myprintk("[%s] eth=%p= skb->data =%p, skb->head = %p=================================\n", desc, eth, skb->data, skb->head);

	if (skb->mark == SESSION_TPROXY_OMARK_VXCLIENT ||
			skb->mark == SESSION_TPROXY_OMARK_SWCLIENT) {
		memcpy(eth_hdr(skb)->h_source, feth1, ETH_ALEN);
		memcpy(eth_hdr(skb)->h_dest, mac_client, ETH_ALEN);
	} else {
		memcpy(eth_hdr(skb)->h_source, feth0, ETH_ALEN);
		memcpy(eth_hdr(skb)->h_dest, mac_server, ETH_ALEN);
	}

	if (type == ETH_P_IP) {
		eth->h_proto = htons(ETH_P_IP); 
	} else {
		eth->h_proto = htons(ETH_P_IPV6); 
		skb->protocol = htons(ETH_P_IPV6);
	}

	rc = dev_queue_xmit(skb);

	myprintk("[%s] ===============================have enqueue, returnd %d\n", desc, rc);
	if (rc < 0) {
		kfree(skb);
		return NF_STOLEN;
	}

	return NF_STOLEN;
}

static unsigned int 
tcp_v4_output_postrouting_last_func(void* hook,
		struct sk_buff *skb,
        const struct nf_hook_state *stat)
{
	const struct iphdr *iph;
#ifdef DEBUG_OUTPUT
	char                source[16];
	char                dest[16];
	char                flags[32];
#endif
	struct rtable *rt = skb_rtable(skb);
	struct net_device *dev = rt->dst.dev;
	struct tcphdr      *th;
	unsigned int        hh_len;
	int                 iif;
	struct sock        *sk;

	if ((skb)->len < sizeof(struct iphdr) ||
			ip_hdrlen(skb) < sizeof(struct iphdr)) {
		return NF_ACCEPT;
	}

	iph = ip_hdr(skb);
	if(iph->protocol != IPPROTO_TCP) {
		return NF_ACCEPT;
	}

	th =  tcp_hdr(skb); 
	if (!filter_br_skb(skb) && 0) {
#ifdef DEBUG_OUTPUT
		if (th->source == ntohs(65535) || th->dest == ntohs(80)) {
			myprintk("[%s] skb->dev->name: %s, skb->mark: %d, skb->skb_iif: %d, skb->sk->sk_bound_dev_if: %d\n", __func__, 
                skb->dev->name, skb->mark, skb->skb_iif, skb->sk->sk_bound_dev_if);

			snprintf(source, 16, "%pI4", &iph->saddr); 
			snprintf(dest, 16, "%pI4", &iph->daddr); 
			memset(flags, 0, sizeof(flags));
			sprintf(flags, "%s%s%s%s%s%s%d", 
					th->syn ? "syn,":"",
					th->ack ? "ack,":"",
					th->psh ? "push,":"",
					th->urg ? "urg,":"",
					th->urg ? "rst,":"",
					th->fin ? "fin,":"",
					skb->data_len);
			myprintk("[%s] tcp %s:%d-> %s:%d  flags: %s\n", __func__, source,
					ntohs(th->source), dest, ntohs(th->dest), flags);
		}
#endif
		return NF_ACCEPT;
	}

	rcu_read_lock();
	hh_len = LL_RESERVED_SPACE(skb->dev);
	if (skb->skb_iif) { // new request
		iif = skb->skb_iif;
		/*myprintk("skb->skb_iif = %d, skb->dev->ifindex = %d\n", skb->skb_iif, skb->dev->ifindex);*/
		if (skb->skb_iif == skb->dev->ifindex) {
			myprintk("[%s] found packet match old output dev to %p, ifindex = %d, name= %s ===============\n",
					__func__, skb->dev, skb->dev->ifindex, skb->dev->name);
			dev = skb->dev;
			goto check_vlan_dev;
		}
	} else { // established socket
		if (unlikely(skb->sk == NULL)) {
			myprintk("[%s] found non socket output skb, drop it\n", __func__);
			rcu_read_unlock();
			return NF_DROP;
		}
		if (skb->priority) {
			myprintk("[%s] found switch mode output skb, sk->sk_bound_dev_if: %d, sk->sk_priority: %d\n",
					__func__, skb->sk->sk_bound_dev_if, skb->priority);
			iif = skb->priority;
			skb->priority = 0;
		} else {
			sk = skb->sk;
			if (sk->sk_bound_dev_if) {
				myprintk("[%s] found output skb, sk->sk_bound_dev_if: %d\n", __func__, skb->sk->sk_bound_dev_if);
				iif = skb->sk->sk_bound_dev_if;
			} else {
				myprintk("[%s] found unbound dev output skb, drop it\n", __func__);
				rcu_read_unlock();
				return NF_DROP;
			}
		}
	}

	myprintk("[%s] old dev: %p, name: %s, ifindex: %d\n",
			__func__, skb->dev, skb->dev->name, skb->dev->ifindex);

	dev = dev_get_by_index_rcu(dev_net(skb->dev), iif);
	if (likely(dev)) {
check_vlan_dev:
		if (is_vlan_dev(dev)) {
			struct net_device *real_dev = vlan_dev_real_dev(dev);
			if (unlikely(real_dev == NULL)) {
				return NF_DROP;
			}
			dev = real_dev;
			myprintk("[%s] tproxy output to vlan dev, change to real dev\n", __func__);
		}

		myprintk("[%s] old LL_RESERVED_SPACE(dev): %d\n", __func__, LL_RESERVED_SPACE(skb->dev));
		myprintk("[%s] change dev to %s\n", __func__, dev->name);
		skb->dev = dev;
		hh_len = LL_RESERVED_SPACE(skb->dev);
	} else {
		myprintk("[%s] can't find output dev\n", __func__);
		rcu_read_unlock();
		return NF_DROP;
	}
	rcu_read_unlock();
	return tcp_output_send_skb(skb, dev, ETH_P_IP, hh_len, __func__);
}

static struct nf_hook_ops tcp_output_hook =
{
		.hook = tcp_v4_output_postrouting_last_func,
		.pf = NFPROTO_IPV4,
		.hooknum = NF_IP_POST_ROUTING,
		.priority = NF_IP_PRI_LAST,
};

static int __init tcp_output_init(void)
{
	printk(KERN_INFO"---init tproxy_hookout_module---\n");
	return nf_register_net_hooks(&init_net, &tcp_output_hook, 1);
}

static void __exit tcp_output_exit(void)
{
	printk(KERN_INFO"---exit tproxy_hookout_module---\n");
	nf_unregister_net_hooks(&init_net, &tcp_output_hook, 1);
}

module_init(tcp_output_init);
module_exit(tcp_output_exit);


MODULE_AUTHOR("souldump");
MODULE_DESCRIPTION("tproxy_hookout_module");

MODULE_LICENSE("GPL");
