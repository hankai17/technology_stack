#pragma once

#define MAX_FILTERS 100
#define ACTION_DROP 0
#define ACTION_ALLOW 1

struct filter
{
    int id;
    int enabled;
    int action; // 0=DROP, 1=ALLOW

    // IP 地址和掩码
    __u8 src_ip[4];
    int src_cidr;

    __u8 dst_ip[4];
    int dst_cidr;

    // 协议类型
    int protocol; // IPPROTO_TCP, IPPROTO_UDP, IPPROTO_ICMP, etc.

    // 端口
    int src_port;
    int dst_port;

    // TTL
    int min_ttl;
    int max_ttl;

    // 数据包长度
    int min_len;
    int max_len;

    // PPS 和 BPS 限制
    unsigned long pps;
    unsigned long bps;

    // 阻止时间
    unsigned long block_time;
};

struct filter_stats
{
    __u64 allowed;
    __u64 dropped;
    __u64 passed;
};
