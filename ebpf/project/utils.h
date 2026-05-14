// utils.h

#pragma once

#include <linux/types.h>

int parse_ip(const char *str, __u32 *ip, int *cidr);
