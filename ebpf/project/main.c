#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <sys/resource.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <net/if.h>
#include <arpa/inet.h>

#include <bpf/libbpf.h>
#include "xdp_filter.skel.h"
#include "xdp_prog_kern.skel.h"

#include "cmdline.h"
#include "config.h"
#include "utils.h"

#define XDP_FLAGS_SKB_MODE    (1U << 1)

static volatile bool running = true;

void sigint_handler(int signo)
{
    running = false;
}

int main(int argc, char **argv)
{
    struct xdp_prog_kern_bpf *skel;
    int err;

    struct cmdline cmd = {0};
    parse_cmdline(&cmd, argc, argv);

    // 设置资源限制
    struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};
    if (setrlimit(RLIMIT_MEMLOCK, &rlim))
    {
        fprintf(stderr, "Failed to set rlimit: %s\n", strerror(errno));
        return EXIT_FAILURE;
    }

    // 加载配置文件
    struct config cfg = {0};
    if (load_config(&cfg, cmd.config_file) != 0)
    {
        fprintf(stderr, "Failed to load config file.\n");
        return EXIT_FAILURE;
    }

    printf("interface: %s\n", cfg.interface);
    // 获取网络接口索引
    int ifindex = if_nametoindex(cfg.interface);
    if (ifindex == 0)
    {
        fprintf(stderr, "Invalid interface name: %s\n", cfg.interface);
        return EXIT_FAILURE;
    }

    // Open and load BPF application
    skel = xdp_prog_kern_bpf__open();                                                                                             
    if (!skel)                                                                                                                  
    {                                                                                                                           
        fprintf(stderr, "Failed to open BPF skeleton\n");                                                                       
        return 1;                                                                                                               
    }                                                                                                                           

    // Load & verify BPF programs 
    err = xdp_prog_kern_bpf__load(skel);
    if (err)
    {
        fprintf(stderr, "Failed to load and verify BPF skeleton: %d\n", err);
        goto cleanup;
    }

    err = xdp_prog_kern_bpf__attach(skel);
    if (err)
    {
        fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);
        goto cleanup;
    }

    err = bpf_xdp_attach(ifindex, -1, XDP_FLAGS_SKB_MODE, NULL);    // deattach first
    if (err) {
        printf("deattach failed\n");
    }

    // Attach the XDP program to the specified interface
    //LIBBPF_OPTS(bpf_xdp_attach_opts, attach_opts);
    //attach_opts.sz = sizeof(struct bpf_xdp_attach_opts);
    //err = bpf_xdp_attach(ifindex,
    //                 bpf_program__fd(skel->progs._xdp_prog_kern),
    //                 XDP_FLAGS_SKB_MODE,
    //                 &attach_opts);

    skel->links._xdp_prog_kern = bpf_program__attach_xdp(skel->progs._xdp_prog_kern, ifindex);
    if (!skel->links._xdp_prog_kern)
    {
        err = -errno;
        fprintf(stderr, "Failed to attach XDP program: %s\n", strerror(errno));
        goto cleanup;
    }
                                                                                                                                
    // Attach XDP program
    //err = xdp_prog_kern_bpf__attach(skel);    // 写了这一行，就不用自己写任何 bpf_program__attach_xdp 了
    //if (err)                                                                                                                    
    //{                                                                                                                           
    //    fprintf(stderr, "Failed to attach BPF skeleton: %d\n", err);                                                            
    //    goto cleanup;                                                                                                           
    //}                                                                                                                           
                                                                                                                                
    // how to set XDP_MODE_NATIVE?
                                                                                                                                
    printf("XDP program loaded successfully on interface %s.\n", cfg.interface);

    // 获取 BPF 映射句柄
    int filters_map_fd = bpf_map__fd(skel->maps.filters_map);
    if (filters_map_fd < 0)
    {
        fprintf(stderr, "Failed to get filters_map FD.\n");
        return EXIT_FAILURE;
    }

    int stats_map_fd = bpf_map__fd(skel->maps.stats_map);
    if (stats_map_fd < 0)
    {
        fprintf(stderr, "Failed to get stats_map FD.\n");
        return EXIT_FAILURE;
    }

    // 更新过滤规则到 BPF 映射
    if (update_filters(filters_map_fd, &cfg) != 0)
    {
        fprintf(stderr, "Failed to update filters.\n");
        return EXIT_FAILURE;
    }

    // 注册信号处理函数
    signal(SIGINT, sigint_handler);

    // 主循环
    while (running)
    {
        // 定期检查配置文件更新
        if (cfg.update_interval > 0 && check_config_update(&cfg))
        {
            if (reload_config(&cfg) == 0)
            {
                update_filters(filters_map_fd, &cfg);
            }
        }

        // 显示统计信息
        if (!cfg.no_stats)
        {
            display_stats(stats_map_fd);
        }

        sleep(cfg.stdout_interval);
    }

    // 卸载 XDP 程序
    xdp_prog_kern_bpf__destroy(skel); 

    printf("XDP program detached and exited.\n");
cleanup:
    return EXIT_SUCCESS;
}
