// cmdline.c

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include "cmdline.h"

static const struct option long_options[] = {
    {"config", required_argument, NULL, 'c'},
    {"offload", no_argument, NULL, 'o'},
    {"skb", no_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

void parse_cmdline(struct cmdline *cmd, int argc, char **argv)
{
    int opt;
    while ((opt = getopt_long(argc, argv, "c:osh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'c':
            cmd->config_file = optarg;
            break;
        case 'o':
            cmd->offload_mode = 1;
            break;
        case 's':
            cmd->skb_mode = 1;
            break;
        case 'h':
            cmd->help = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [--config CONFIG_FILE] [--offload] [--skb] [--help]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if (cmd->help)
    {
        printf("Usage: %s [--config CONFIG_FILE] [--offload] [--skb] [--help]\n", argv[0]);
        exit(EXIT_SUCCESS);
    }
    if (!cmd->config_file)
    {
        cmd->config_file = "/etc/xdpfw/xdpfw.conf"; // 默认配置文件路径
    }
}