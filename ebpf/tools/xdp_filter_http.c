#include <linux/bpf.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define MAX_ENTRIES 4096
#define SSH_PORT 22
#define SAMPLE_RATE 2048
#define HTTP_PORT 80
#define HTTPS_PORT 443
#define MAX_HTTP_HEADER_SIZE 256
#define DEFAULT_SAMPLE_SIZE 64

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

struct sample_data
{
    __u64 timestamp;
    __u16 sample_size;
    __u8 data[MAX_HTTP_HEADER_SIZE];
    __u8 is_http;
    __u8 is_ipv6;
    union
    {
        __u32 ipv4;
        __u8 ipv6[16];
    } src_ip;
    __u16 src_port;
    union
    {
        __u32 ipv4;
        __u8 ipv6[16];
    } dst_ip;
    __u16 dst_port;
};

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __uint(key_size, sizeof(struct v4_key));
    __uint(value_size, sizeof(__u32));
} whitelist_v4 SEC(".maps");

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
} log_buffer SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 512 * 1024);
} sample_buffer SEC(".maps");

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
        __builtin_memcpy(entry.src_ip.ipv6, src_ip6, sizeof(entry.src_ip.ipv6));
    }
    else
    {
        entry.src_ip.ipv4 = src_ip;
    }

    bpf_ringbuf_output(&log_buffer, &entry, sizeof(entry), 0);
}

static __always_inline void sample_packet(void *data, void *data_end, struct iphdr *iph, struct ipv6hdr *ip6h, struct tcphdr *tcph)
{
    struct sample_data sample = {};
    sample.timestamp = bpf_ktime_get_ns();

    if (iph)
    {
        sample.is_ipv6 = 0;
        sample.src_ip.ipv4 = iph->saddr;
        sample.dst_ip.ipv4 = iph->daddr;
    }
    else if (ip6h)
    {
        sample.is_ipv6 = 1;
        __builtin_memcpy(sample.src_ip.ipv6, ip6h->saddr.s6_addr, sizeof(sample.src_ip.ipv6));
        __builtin_memcpy(sample.dst_ip.ipv6, ip6h->daddr.s6_addr, sizeof(sample.dst_ip.ipv6));
    }
    else
    {
        return;
    }

    if (tcph)
    {
        sample.src_port = bpf_ntohs(tcph->source);
        sample.dst_port = bpf_ntohs(tcph->dest);
        if (sample.dst_port == HTTP_PORT || sample.dst_port == HTTPS_PORT)
        {
            sample.is_http = 1;
            sample.sample_size = MAX_HTTP_HEADER_SIZE;
        }
        else
        {
            sample.sample_size = DEFAULT_SAMPLE_SIZE;
        }
    }
    else
    {
        sample.sample_size = DEFAULT_SAMPLE_SIZE;
    }

    if (data + sample.sample_size > data_end)
    {
        sample.sample_size = data_end - data;
    }

    __builtin_memcpy(sample.data, data, sample.sample_size);

    bpf_ringbuf_output(&sample_buffer, &sample, sizeof(sample), 0);
}

static __always_inline int process_tcp(void *data, void *data_end, struct tcphdr *tcph, struct ip_stats *stats, struct iphdr *iph, struct ipv6hdr *ip6h)
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

    // 流量抽样
    if (stats->packets % SAMPLE_RATE == 0)
    {
        sample_packet(data, data_end, iph, ip6h, tcph);
    }

    return XDP_PASS;
}

static __always_inline int process_ipv4(struct xdp_md *ctx, struct iphdr *iph)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct v4_key key = {.addr = iph->saddr};

    // 检查白名单
    __u32 *whitelisted = bpf_map_lookup_elem(&whitelist_v4, &key);
    if (whitelisted)
    {
        return XDP_PASS;
    }

    // 检查黑名单
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
    if (diff >= 1000000000)
    { // 1 second in nanoseconds
        if (stats->packets > stats->max_packets_per_sec ||
            stats->bytes > stats->max_bytes_per_sec ||
            stats->syn_count > stats->max_syn_per_sec)
        {
            log_drop(iph->saddr, NULL, DROP_RATE_LIMIT, stats->packets, 0);
        }
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
        struct tcphdr *tcph = (void *)iph + sizeof(*iph);
        return process_tcp(data, data_end, tcph, stats, iph, NULL);
    }
    else
    {
        // 非TCP流量抽样
        if (stats->packets % SAMPLE_RATE == 0)
        {
            sample_packet(data, data_end, iph, NULL, NULL);
        }
    }

    return XDP_PASS;
}

static __always_inline int process_ipv6(struct xdp_md *ctx, struct ipv6hdr *ip6h)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct v6_key key;
    __builtin_memcpy(key.addr, ip6h->saddr.s6_addr, sizeof(key.addr));

    // 检查白名单
    __u32 *whitelisted = bpf_map_lookup_elem(&whitelist_v6, &key);
    if (whitelisted)
    {
        return XDP_PASS;
    }

    // 检查黑名单
    __u32 *blacklisted = bpf_map_lookup_elem(&blacklist_v6, &key);
    if (blacklisted)
    {
        log_drop(0, ip6h->saddr.s6_addr, DROP_BLACKLIST, 1, 1);
        return XDP_DROP;
    }

    struct ip_stats *stats = bpf_map_lookup_elem(&rate_limit_v6, &key);

    if (!stats)
    {
        return XDP_PASS;
    }

    __u64 now = bpf_ktime_get_ns();
    __u64 diff = now - stats->last_update;
    if (diff >= 1000000000)
    { // 1 second in nanoseconds
        if (stats->packets > stats->max_packets_per_sec ||
            stats->bytes > stats->max_bytes_per_sec ||
            stats->syn_count > stats->max_syn_per_sec)
        {
            log_drop(0, ip6h->saddr.s6_addr, DROP_RATE_LIMIT, stats->packets, 1);
        }
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
            log_drop(0, ip6h->saddr.s6_addr, DROP_RATE_LIMIT, 1, 1);
            return XDP_DROP;
        }
    }

    if (ip6h->nexthdr == IPPROTO_TCP)
    {
        struct tcphdr *tcph = (void *)ip6h + sizeof(*ip6h);
        return process_tcp(data, data_end, tcph, stats, NULL, ip6h);
    }
    else
    {
        // 非TCP流量抽样
        if (stats->packets % SAMPLE_RATE == 0)
        {
            sample_packet(data, data_end, NULL, ip6h, NULL);
        }
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
        struct iphdr *iph = (void *)(eth + 1);
        if ((void *)(iph + 1) > data_end)
            return XDP_PASS;
        return process_ipv4(ctx, iph);
    }
    else if (h_proto == ETH_P_IPV6)
    {
        struct ipv6hdr *ip6h = (void *)(eth + 1);
        if ((void *)(ip6h + 1) > data_end)
            return XDP_PASS;
        return process_ipv6(ctx, ip6h);
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
