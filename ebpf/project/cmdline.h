// cmdline.h

#pragma once

struct cmdline
{
    char *config_file;
    int offload_mode;
    int skb_mode;
    int help;
};

void parse_cmdline(struct cmdline *cmd, int argc, char **argv);