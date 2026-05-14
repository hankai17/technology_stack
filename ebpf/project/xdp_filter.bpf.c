#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>          // 安全读取内核内存
#include <bpf/bpf_endian.h>

#define MAX_ENTRIES 4096
#define SSH_PORT 22
#define ETH_P_IP        0x0800
#define ETH_P_IPV6      0x86DD
#define IPPROTO_TCP     6

struct v4_key
{
    __u32 addr;
};

struct v6_key
{
    __u8 addr[16];
};

struct ip_stats
{
    __u64 packets;
    __u64 bytes;
    __u64 syn_count;
    __u64 last_update;
    __u32 max_packets_per_sec;
    __u32 max_bytes_per_sec;
    __u32 max_syn_per_sec;
};

struct log_entry
{
    __u64 timestamp;
    union
    {
        __u32 ipv4;
        __u8 ipv6[16];
    } src_ip;
    __u32 reason;
    __u32 drop_count;
    __u8 is_ipv6;
};

struct                                          // 匿名结构体，专门用于声明 eBPF map
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v4_key));
    __uint(value_size, sizeof(__u32));
} whitelist_v4 SEC(".maps");                    // 把这个 map 放入 ELF 的 .maps 段（eBPF 加载器识别）

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v6_key));
    __uint(value_size, sizeof(__u32));
} whitelist_v6 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v4_key));
    __uint(value_size, sizeof(__u32));
} blacklist_v4 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v6_key));
    __uint(value_size, sizeof(__u32));
} blacklist_v6 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v4_key));
    __uint(value_size, sizeof(struct ip_stats));
} rate_limit_v4 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v6_key));
    __uint(value_size, sizeof(struct ip_stats));
} rate_limit_v6 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
    //__uint(pinning, LIBBPF_PIN_BY_NAME); // pin到内核级共享内存: ebpf程序退出了 map还在数据不丢
} log_buffer SEC(".maps");

#define DROP_BLACKLIST 1
#define DROP_RATE_LIMIT 2

static __always_inline void log_drop(__u32 src_ip, __u8 *src_ip6, __u32 reason, __u32 drop_count, __u8 is_ipv6)
{
    struct log_entry entry = {};
    entry.timestamp = bpf_ktime_get_ns();
    entry.reason = reason;
    entry.drop_count = drop_count;
    entry.is_ipv6 = is_ipv6;

    if (is_ipv6)
    {
        __builtin_memcpy(entry.src_ip.ipv6, src_ip6, 16);
    }
    else
    {
        entry.src_ip.ipv4 = src_ip;
    }

    bpf_ringbuf_output(&log_buffer, &entry, sizeof(entry), 0);
}

static __always_inline int process_tcp(void *data, void *data_end, struct tcphdr *tcph, struct ip_stats *stats)
{
    if ((void *)(tcph + 1) > data_end)
        return XDP_PASS;

    __u16 dport = bpf_ntohs(tcph->dest);

    if (dport == SSH_PORT)
    {
        return XDP_PASS;
    }

    if (tcph->syn && !tcph->ack)
    {
        stats->syn_count++;
    }

    return XDP_PASS;
}

static __always_inline int process_ipv4(struct xdp_md *ctx, struct iphdr *iph)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct v4_key key = {.addr = iph->saddr};

    __u32 *whitelisted = bpf_map_lookup_elem(&whitelist_v4, &key);
    if (whitelisted)
    {
        return XDP_PASS;
    }

    __u32 *blacklisted = bpf_map_lookup_elem(&blacklist_v4, &key);
    if (blacklisted)
    {
        log_drop(iph->saddr, NULL, DROP_BLACKLIST, 1, 0);
        return XDP_DROP;
    }

    struct ip_stats *stats = bpf_map_lookup_elem(&rate_limit_v4, &key);
    if (!stats)
    {
        return XDP_PASS;
    }

    __u64 now = bpf_ktime_get_ns();
    __u64 diff = now - stats->last_update;

    if (diff >= 1000000000)                     // 1s in ns
    {
        stats->packets = 1;
        stats->bytes = data_end - data;
        stats->syn_count = 0;
        stats->last_update = now;
    }
    else
    {
        stats->packets++;
        stats->bytes += data_end - data;

        if (stats->packets > stats->max_packets_per_sec ||
            stats->bytes > stats->max_bytes_per_sec ||
            stats->syn_count > stats->max_syn_per_sec)
        {
            log_drop(iph->saddr, NULL, DROP_RATE_LIMIT, 1, 0);
            return XDP_DROP;
        }
    }

    if (iph->protocol == IPPROTO_TCP)
    {
        struct tcphdr *tcph = (struct tcphdr *)(iph + 1);
        return process_tcp(data, data_end, tcph, stats);
    }

    return XDP_PASS;
}

static __always_inline int process_ipv6(struct xdp_md *ctx, struct ipv6hdr *ip6h)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct v6_key key;

    __builtin_memcpy(key.addr, ip6h->saddr.in6_u.u6_addr8, 16);

    __u32 *whitelisted = bpf_map_lookup_elem(&whitelist_v6, &key);
    if (whitelisted)
        return XDP_PASS;

    __u32 *blacklisted = bpf_map_lookup_elem(&blacklist_v6, &key);
    if (blacklisted)
    {
        log_drop(0, ip6h->saddr.in6_u.u6_addr8, DROP_BLACKLIST, 1, 1);
        return XDP_DROP;
    }

    struct ip_stats *stats = bpf_map_lookup_elem(&rate_limit_v6, &key);
    if (!stats)
        return XDP_PASS;

    __u64 now = bpf_ktime_get_ns();
    __u64 diff = now - stats->last_update;

    if (diff >= 1000000000)
    {
        stats->packets = 1;
        stats->bytes = data_end - data;
        stats->syn_count = 0;
        stats->last_update = now;
    }
    else
    {
        stats->packets++;
        stats->bytes += data_end - data;

        if (stats->packets > stats->max_packets_per_sec ||
            stats->bytes > stats->max_bytes_per_sec ||
            stats->syn_count > stats->max_syn_per_sec)
        {
            log_drop(0, ip6h->saddr.in6_u.u6_addr8, DROP_RATE_LIMIT, 1, 1);
            return XDP_DROP;
        }
    }

    if (ip6h->nexthdr == IPPROTO_TCP)
    {
        struct tcphdr *tcph = (struct tcphdr *)(ip6h + 1);
        return process_tcp(data, data_end, tcph, stats);
    }

    return XDP_PASS;
}

SEC("xdp")
int xdp_filter(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;
    struct ethhdr *eth = data;

    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    __u16 h_proto = bpf_ntohs(eth->h_proto);

    if (h_proto == ETH_P_IP)
    {
        struct iphdr *iph = (struct iphdr *)(eth + 1);
        if ((void *)(iph + 1) > data_end)
            return XDP_PASS;
        return process_ipv4(ctx, iph);
    }
    else if (h_proto == ETH_P_IPV6)
    {
        struct ipv6hdr *ip6h = (struct ipv6hdr *)(eth + 1);
        if ((void *)(ip6h + 1) > data_end)
            return XDP_PASS;
        return process_ipv6(ctx, ip6h);
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
