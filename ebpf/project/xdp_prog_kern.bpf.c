#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>
#include <bpf/bpf_tracing.h>
#include "xdpfw.h"

#define SSH_PORT 22
#define ETH_P_IP        0x0800
#define ETH_P_IPV6      0x86DD
#define IPPROTO_TCP     6

struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, MAX_FILTERS);
    __type(key, __u32);
    __type(value, struct filter);
} filters_map SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, struct filter_stats);
} stats_map SEC(".maps");

SEC("xdp")
int _xdp_prog_kern(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    __u16 h_proto = bpf_ntohs(eth->h_proto);

    // 获取统计映射
    __u32 stats_key = 0;
    struct filter_stats *st = bpf_map_lookup_elem(&stats_map, &stats_key);

    if (!st)
        return XDP_PASS;

    // 初始化动作为 XDP_PASS
    int action = XDP_PASS;

    // 数据包长度
    int pkt_len = data_end - data;

    // 初始化协议和IP信息
    __u8 ip_proto = 0;
    __u32 src_ip = 0;
    __u32 dst_ip = 0;
    __u16 src_port = 0;
    __u16 dst_port = 0;
    __u8 ttl = 0;

    if (h_proto == ETH_P_IP)
    {
        struct iphdr *iph = data + sizeof(struct ethhdr);
        if ((void *)(iph + 1) > data_end)
            return XDP_PASS;

        ip_proto = iph->protocol;
        src_ip = iph->saddr;
        dst_ip = iph->daddr;
        ttl = iph->ttl;

        // 处理 TCP/UDP 端口
        if (ip_proto == IPPROTO_TCP)
        {
            struct tcphdr *tcph = (void *)(iph + 1);
            if ((void *)(tcph + 1) > data_end)
                return XDP_PASS;
            src_port = bpf_ntohs(tcph->source);
            dst_port = bpf_ntohs(tcph->dest);
        }
        else if (ip_proto == IPPROTO_UDP)
        {
            struct udphdr *udph = (void *)(iph + 1);
            if ((void *)(udph + 1) > data_end)
                return XDP_PASS;
            src_port = bpf_ntohs(udph->source);
            dst_port = bpf_ntohs(udph->dest);
        }
    }
    else if (h_proto == ETH_P_IPV6)
    {
        // 省略对 IPv6 的处理，可以根据需要添加
        return XDP_PASS;
    }
    else
    {
        return XDP_PASS;
    }

    // 遍历过滤规则
    for (__u32 i = 0; i < MAX_FILTERS; i++)
    {
        struct filter *flt = bpf_map_lookup_elem(&filters_map, &i);
        if (!flt || !flt->enabled)
            continue;

        // 匹配源IP
        if (flt->src_ip[0] != 0)
        {
            if (*(__be32 *)flt->src_ip == src_ip)
            {
                continue;
            }
        }

        // 匹配目的IP
        if (flt->dst_ip[0] != 0)
        {
            if (*(__be32 *)flt->dst_ip == dst_ip)
            {
                continue;
            }
        }

        // 匹配协议
        if (flt->protocol != 0 && flt->protocol != ip_proto)
        {
            continue;
        }

        // 匹配端口
        if (flt->src_port != 0 && flt->src_port != src_port)
        {
            continue;
        }
        if (flt->dst_port != 0 && flt->dst_port != dst_port)
        {
            continue;
        }

        // 匹配TTL
        if (flt->min_ttl != 0 && ttl < flt->min_ttl)
        {
            continue;
        }
        if (flt->max_ttl != 0 && ttl > flt->max_ttl)
        {
            continue;
        }

        // 匹配长度
        if (flt->min_len != 0 && pkt_len < flt->min_len)
        {
            continue;
        }
        if (flt->max_len != 0 && pkt_len > flt->max_len)
        {
            continue;
        }

        // 匹配成功
        if (flt->action == ACTION_DROP)
        {
            action = XDP_DROP;
            break;
        }
        else if (flt->action == ACTION_ALLOW)
        {
            action = XDP_PASS;
            break;
        }
    }

    // 更新统计信息
    if (action == XDP_DROP)
    {
        st->dropped++;
    }
    else if (action == XDP_PASS)
    {
        st->passed++;
    }
    else
    {
        st->allowed++;
    }

    return action;
}

char _license[] SEC("license") = "GPL";
