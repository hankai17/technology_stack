// config.h

#pragma once

#include <time.h>
#include <stdint.h>
#include <linux/types.h>
#include "xdpfw.h"

#define MAX_FILTERS 100

struct config
{
    char *interface;
    int update_interval;
    int no_stats;
    int stdout_interval;
    int filter_count;
    struct filter filters[MAX_FILTERS];
    time_t last_modified;
    char *config_file;
};

int load_config(struct config *cfg, const char *filename);
int reload_config(struct config *cfg);
int check_config_update(struct config *cfg);
int update_filters(int filters_map_fd, struct config *cfg);
void display_stats(int stats_map_fd);
