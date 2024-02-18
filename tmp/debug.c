#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#define OUT_FILE "/home/debug.txt"
#define WAF_LOG_DEBUG(fmt,...)\
do{ \
    char cmd[512+100] = {0}; \
    char tmp[512] = {0}; \
    static time_t ngx_now;\
    time(&ngx_now);\
    struct tm * ngx_curTime;\
    ngx_curTime = localtime(&ngx_now); \
    snprintf(tmp, 512,"%d-%d-%d %2d:%2d:%2d   Func:%s Len:%d ------  "fmt,\
            ngx_curTime->tm_year+1900,ngx_curTime->tm_mon+1,ngx_curTime->tm_mday, \
            ngx_curTime->tm_hour,ngx_curTime->tm_min, ngx_curTime->tm_sec, \
            __FUNCTION__,__LINE__ ,##__VA_ARGS__);\
    snprintf(cmd, 512+100, "echo %s >> %s", tmp, OUT_FILE); \
    system(cmd);\
}while(0)

int main()
{
    WAF_LOG_DEBUG("%s\n", "he");
    return 0;
}
