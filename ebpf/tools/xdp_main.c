#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <linux/if_ether.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <bpf/bpf_endian.h>

// 定义常量
#define MAX_SAMPLE_SIZE 256
#define MAX_SAMPLE_RATE 1000000
#define MAX_ENTRIES 4096
#define SSH_PORT 22

#define DROP_BLACKLIST 1
#define DROP_RATE_LIMIT 2

// 定义控制标志结构
struct control_flags
{
    __u32 sampling_enabled;
    __u32 xdpcap_enabled;
};

// 定义抽样配置结构
struct sample_config
{
    __u32 sample_rate;
    __u32 sample_size;
};

// 定义抽样数据结构
struct sample_data
{
    __u64 timestamp;
    __u32 pkt_size;
    __u8 data[MAX_SAMPLE_SIZE];
};

// 定义IP键结构
struct v4_key
{
    __u32 addr;
};

struct v6_key
{
    __u8 addr[16];
};

// 定义IP统计信息结构
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

// 定义日志条目结构
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

// 定义控制映射
struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, __u32);
    __type(value, struct control_flags);
    __uint(max_entries, 1);
} control_map SEC(".maps");

// 定义抽样配置映射
struct
{
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __type(key, __u32);
    __type(value, struct sample_config);
    __uint(max_entries, 1);
} config_map SEC(".maps");

// 定义抽样数据的环形缓冲区
struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 4 * 1024 * 1024);
} rb SEC(".maps");

// 定义xdpcap钩子映射
struct
{
    __uint(type, BPF_MAP_TYPE_PROG_ARRAY);
    __uint(max_entries, 1);
    __type(key, __u32);
    __type(value, __u32);
} xdpcap_hook SEC(".maps");

// 定义白名单和黑名单映射
struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v4_key);
    __type(value, __u32);
} whitelist_v4 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v6_key);
    __type(value, __u32);
} whitelist_v6 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v4_key);
    __type(value, __u32);
} blacklist_v4 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v6_key);
    __type(value, __u32);
} blacklist_v6 SEC(".maps");

// 定义速率限制映射
struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v4_key);
    __type(value, struct ip_stats);
} rate_limit_v4 SEC(".maps");

struct
{
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, MAX_ENTRIES);
    __type(key, struct v6_key);
    __type(value, struct ip_stats);
} rate_limit_v6 SEC(".maps");

// 定义日志环形缓冲区
struct
{
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
    __uint(pinning, LIBBPF_PIN_BY_NAME);
} log_buffer SEC(".maps");

// xdpcap退出函数
static inline int xdpcap_exit(void *ctx, void *map, int index)
{
    return ((int (*)(void *, void *, int))bpf_tail_call)(ctx, map, index);
}

// 抽样包函数
static __always_inline int sample_packet(struct xdp_md *ctx, const struct sample_config *config)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    // 简单的随机抽样
    if (bpf_get_prandom_u32() % config->sample_rate != 0)
    {
        return 0;
    }

    // 准备抽样数据
    struct sample_data *sd = bpf_ringbuf_reserve(&rb, sizeof(*sd), 0);
    if (!sd)
    {
        return 0; // Ring buffer 满，跳过抽样
    }

    // 填充抽样数据
    sd->timestamp = bpf_ktime_get_ns();
    sd->pkt_size = (__u32)(data_end - data);

    __u32 sample_size = config->sample_size;
    if (sample_size > MAX_SAMPLE_SIZE)
    {
        sample_size = MAX_SAMPLE_SIZE;
    }
    if (sample_size > sd->pkt_size)
    {
        sample_size = sd->pkt_size;
    }

    // 复制数据包内容
    if (bpf_xdp_load_bytes(ctx, 0, sd->data, sample_size) != 0)
    {
        bpf_ringbuf_discard(sd, 0);
        return 0;
    }

    // 提交抽样数据
    bpf_ringbuf_submit(sd, 0);
    return 0;
}

// 日志记录函数
static __always_inline void log_drop(__u32 src_ip, __u8 *src_ip6, __u32 reason, __u32 drop_count, __u8 is_ipv6)
{
    struct log_entry *entry = bpf_ringbuf_reserve(&log_buffer, sizeof(struct log_entry), 0);
    if (!entry)
    {
        return; // 环形缓冲区满，跳过日志记录
    }
    entry->timestamp = bpf_ktime_get_ns();
    entry->reason = reason;
    entry->drop_count = drop_count;
    entry->is_ipv6 = is_ipv6;

    if (is_ipv6)
    {
        __builtin_memcpy(entry->src_ip.ipv6, src_ip6, sizeof(entry->src_ip.ipv6));
    }
    else
    {
        entry->src_ip.ipv4 = src_ip;
    }

    bpf_ringbuf_submit(entry, 0);
}

// 处理TCP包
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

// 处理IPv4包
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
    {
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
        return process_tcp(data, data_end, tcph, stats);
    }

    return XDP_PASS;
}

// 处理IPv6包
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
    {
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
        return process_tcp(data, data_end, tcph, stats);
    }

    return XDP_PASS;
}

// 过滤逻辑函数
static __always_inline int xdp_filter_logic(struct xdp_md *ctx)
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

// 主XDP程序
SEC("xdp")
int xdp_prog_sample(struct xdp_md *ctx)
{
    __u32 key = 0;
    const struct control_flags *control = bpf_map_lookup_elem(&control_map, &key);

    // 应用过滤逻辑
    int action = xdp_filter_logic(ctx);

    if (action != XDP_DROP)
    {
        // 如果启用了抽样，进行抽样
        if (control && control->sampling_enabled)
        {
            const struct sample_config *config = bpf_map_lookup_elem(&config_map, &key);
            if (config && config->sample_rate > 0 && config->sample_rate <= MAX_SAMPLE_RATE)
            {
                sample_packet(ctx, config);
            }
        }
    }

    // 如果启用了xdpcap，执行tail call
    if (control && control->xdpcap_enabled)
    {
        return xdpcap_exit(ctx, &xdpcap_hook, action);
    }

    return action;
}

char _license[] SEC("license") = "GPL";
