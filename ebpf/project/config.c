#include "config.h"
#include <libconfig.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <bpf/bpf.h>
#include <bpf/libbpf.h>

#include <arpa/inet.h>
#include <errno.h>
#include "utils.h"

#include "xdpfw.h"

int load_config(struct config *cfg, const char *filename)
{
    config_t cfgfile;
    config_init(&cfgfile);

    if (!config_read_file(&cfgfile, filename))
    {
        fprintf(stderr, "Config file error: %s:%d - %s\n",
                config_error_file(&cfgfile),
                config_error_line(&cfgfile),
                config_error_text(&cfgfile));
        config_destroy(&cfgfile);
        return -1;
    }

    cfg->config_file = strdup(filename);

    // 解析配置项
    if (!config_lookup_string(&cfgfile, "interface", (const char **)&cfg->interface))
    {
        fprintf(stderr, "No 'interface' setting in configuration file.\n");
        config_destroy(&cfgfile);
        return -1;
    }

    // update_interval
    if (!config_lookup_int(&cfgfile, "update_interval", &cfg->update_interval))
    {
        cfg->update_interval = 10; // 默认10秒
    }

    // no_stats
    int no_stats = 0;
    if (config_lookup_bool(&cfgfile, "no_stats", &no_stats))
    {
        cfg->no_stats = no_stats;
    }
    else
    {
        cfg->no_stats = 0; // 默认显示统计信息
    }

    // stdout_interval
    if (!config_lookup_int(&cfgfile, "stdout_interval", &cfg->stdout_interval))
    {
        cfg->stdout_interval = 1; // 默认1秒输出一次统计信息
    }

    // 解析过滤规则列表
    config_setting_t *filters_setting = config_lookup(&cfgfile, "filters");
    if (filters_setting != NULL)
    {
        int filter_count = config_setting_length(filters_setting);
        cfg->filter_count = (filter_count < MAX_FILTERS) ? filter_count : MAX_FILTERS;

        for (int i = 0; i < cfg->filter_count; i++)
        {
            config_setting_t *filter_setting = config_setting_get_elem(filters_setting, i);
            struct filter *flt = &cfg->filters[i];

            flt->id = i;

            if (!config_setting_lookup_bool(filter_setting, "enabled", &flt->enabled))
            {
                flt->enabled = 1; // 默认启用
            }

            if (!config_setting_lookup_int(filter_setting, "action", &flt->action))
            {
                flt->action = ACTION_DROP; // 默认动作
            }

            // 解析源IP和CIDR
            const char *src_ip = NULL;
            if (config_setting_lookup_string(filter_setting, "src_ip", &src_ip))
            {
                strncpy((char*)flt->src_ip, src_ip, sizeof(flt->src_ip));
                flt->src_cidr = 32; // 默认CIDR
                // 检查是否有CIDR
                char *slash = strchr((char*)flt->src_ip, '/');
                if (slash)
                {
                    *slash = '\0';
                    flt->src_cidr = atoi(slash + 1);
                }
            }
            else
            {
                flt->src_ip[0] = '\0';
                flt->src_cidr = 0;
            }

            // 解析目标IP和CIDR
            const char *dst_ip = NULL;
            if (config_setting_lookup_string(filter_setting, "dst_ip", &dst_ip))
            {
                strncpy((char*)flt->dst_ip, dst_ip, sizeof(flt->dst_ip));
                flt->dst_cidr = 32; // 默认CIDR
                // 检查是否有CIDR
                char *slash = strchr((char*)flt->dst_ip, '/');
                if (slash)
                {
                    *slash = '\0';
                    flt->dst_cidr = atoi(slash + 1);
                }
            }
            else
            {
                flt->dst_ip[0] = '\0';
                flt->dst_cidr = 0;
            }

            // 解析协议
            const char *protocol = NULL;
            if (config_setting_lookup_string(filter_setting, "protocol", &protocol))
            {
                if (strcmp(protocol, "TCP") == 0)
                    flt->protocol = IPPROTO_TCP;
                else if (strcmp(protocol, "UDP") == 0)
                    flt->protocol = IPPROTO_UDP;
                else if (strcmp(protocol, "ICMP") == 0)
                    flt->protocol = IPPROTO_ICMP;
                else
                    flt->protocol = 0; // 不指定协议
            }
            else
            {
                flt->protocol = 0;
            }

            // 解析端口
            if (!config_setting_lookup_int(filter_setting, "src_port", &flt->src_port))
            {
                flt->src_port = 0;
            }
            if (!config_setting_lookup_int(filter_setting, "dst_port", &flt->dst_port))
            {
                flt->dst_port = 0;
            }

            // 解析TTL和长度
            if (!config_setting_lookup_int(filter_setting, "min_ttl", &flt->min_ttl))
            {
                flt->min_ttl = 0;
            }
            if (!config_setting_lookup_int(filter_setting, "max_ttl", &flt->max_ttl))
            {
                flt->max_ttl = 255;
            }
            if (!config_setting_lookup_int(filter_setting, "min_len", &flt->min_len))
            {
                flt->min_len = 0;
            }
            if (!config_setting_lookup_int(filter_setting, "max_len", &flt->max_len))
            {
                flt->max_len = 0xFFFF;
            }

            // 解析PPS和BPS
            if (!config_setting_lookup_int64(filter_setting, "pps", (long long *)&flt->pps))
            {
                flt->pps = 0;
            }
            if (!config_setting_lookup_int64(filter_setting, "bps", (long long *)&flt->bps))
            {
                flt->bps = 0;
            }

            // 解析阻止时间
            if (!config_setting_lookup_int64(filter_setting, "block_time", (long long *)&flt->block_time))
            {
                flt->block_time = 60; // 默认阻止60秒
            }
        }
    }
    else
    {
        cfg->filter_count = 0;
    }

    // 记录配置文件修改时间
    struct stat st;
    if (stat(filename, &st) == 0)
    {
        cfg->last_modified = st.st_mtime;
    }

    /*
    config_destroy(&cfgfile);
    */
    return 0;
}

int reload_config(struct config *cfg)
{
    return load_config(cfg, cfg->config_file);
}

int check_config_update(struct config *cfg)
{
    struct stat st;
    if (stat(cfg->config_file, &st) == 0)
    {
        if (st.st_mtime != cfg->last_modified)
        {
            cfg->last_modified = st.st_mtime;
            return 1;
        }
    }
    return 0;
}

int update_filters(int filters_map_fd, struct config *cfg)
{
    // 清空现有映射
    for (__u32 i = 0; i < MAX_FILTERS; i++)
    {
        bpf_map_delete_elem(filters_map_fd, &i);
    }

    for (int i = 0; i < cfg->filter_count; i++)
    {
        struct filter *flt = &cfg->filters[i];
        __u32 key = i;

        if (flt->enabled)
        {
            if (bpf_map_update_elem(filters_map_fd, &key, flt, BPF_ANY) != 0)
            {
                fprintf(stderr, "Failed to update filter at index %d: %s\n", i, strerror(errno));
                return -1;
            }
        }
    }
    return 0;
}

void display_stats(int stats_map_fd)
{
    struct filter_stats total_stats = {0};
    __u32 key = 0;

    int num_cpus = libbpf_num_possible_cpus();
    struct filter_stats *per_cpu_stats = malloc(sizeof(struct filter_stats) * num_cpus);

    if (!per_cpu_stats)
    {
        fprintf(stderr, "Failed to allocate memory for per-CPU stats.\n");
        return;
    }

    if (bpf_map_lookup_elem(stats_map_fd, &key, per_cpu_stats) == -1)   // POD 深拷贝
    {
        fprintf(stderr, "Failed to read stats from map.\n");
        free(per_cpu_stats);
        return;
    }

    for (int i = 0; i < num_cpus; i++)
    {
        total_stats.allowed += per_cpu_stats[i].allowed;
        total_stats.dropped += per_cpu_stats[i].dropped;
        total_stats.passed += per_cpu_stats[i].passed;
    }

    printf("Allowed: %llu, Dropped: %llu, Passed: %llu\n",
           total_stats.allowed, total_stats.dropped, total_stats.passed);

    free(per_cpu_stats);
}
