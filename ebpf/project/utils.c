// utils.c

#include "utils.h"
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

int parse_ip(const char *str, __u32 *ip, int *cidr)
{
    char ip_str[40];
    strncpy(ip_str, str, sizeof(ip_str));
    ip_str[39] = '\0';

    char *slash = strchr(ip_str, '/');
    if (slash)
    {
        *slash = '\0';
        *cidr = atoi(slash + 1);
    }
    else
    {
        *cidr = 32;
    }

    if (inet_pton(AF_INET, ip_str, ip) != 1)
    {
        return -1;
    }
    *ip = htonl(*ip);
    return 0;
}