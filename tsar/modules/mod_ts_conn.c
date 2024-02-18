#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "tsar.h"
#include <unistd.h>
#include <fcntl.h>

const static short int TS_RECORD_GET = 3;
const static int LINE_1024 = 1024;
const static int LINE_4096 = 4096;
const static char *RECORDS_NAME[]= {
    "proxy.process.http.current_client_connections",
    "proxy.process.http.current_server_connections",
    "proxy.process.http.current_cache_connections",
    "proxy.process.net.connections_currently_open",
    "proxy.process.http.current_active_client_connections",
    "proxy.process.http.current_client_transactions",
    "proxy.process.http.current_server_transactions"
};
const static char *sock_path = "/opt/ats/var/trafficserver/mgmtapi.sock";

/*
 * Structure for TS information
 */
struct stats_ts_conn {
    unsigned long long c_client;
    unsigned long long c_server;
    unsigned long long c_cache;
    unsigned long long c_open;
    unsigned long long c_a_client;
    unsigned long long t_client;
    unsigned long long t_server;
};

static char *ts_conn_usage = "    --ts_conn           trafficserver connection statistics";

static struct mod_info ts_conn_info[] = {
    {"client", DETAIL_BIT, 0, STATS_NULL},
    {"server", DETAIL_BIT, 0, STATS_NULL},
    {" cache", DETAIL_BIT, 0, STATS_NULL},
    {"  open", DETAIL_BIT, 0, STATS_NULL},
    {" c_act", DETAIL_BIT, 0, STATS_NULL},
    {" t_cli", DETAIL_BIT, 0, STATS_NULL},
    {" t_srv", DETAIL_BIT, 0, STATS_NULL},
};

void
set_ts_conn_record(struct module *mod, double st_array[],
    U_64 pre_array[], U_64 cur_array[], int inter)
{
    int i;
    for (i = 0; i < 7; i++) {
        st_array[i] = cur_array[i];
    }
}
/*
void
read_ts_conn_stats(struct module *mod)
{
    int                   fd = -1;
    int                   pos;
    char                  buf[LINE_4096];
    struct sockaddr_un    un;
    struct stats_ts_conn  st_ts;
    bzero(&st_ts, sizeof(st_ts));
    bzero(&un, sizeof(un));
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        goto done;
    }
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, sock_path);
    if (connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        goto done;
    }

    int          i, len;
    int          record_len = sizeof(RECORDS_NAME) / sizeof(RECORDS_NAME[0]);
    const char  *info;

    for ( i = 0; i < record_len; ++i) {
        info = RECORDS_NAME[i];
        char       write_buf[LINE_1024];
        long int   info_len = strlen(info);
        short int  command = TS_RECORD_GET;

        *((short int *)&write_buf[0]) = command;
        *((long int *)&write_buf[2]) = info_len;
        strcpy(write_buf + 6, info);
        len = 2 + 4 + strlen(info);
        if (write(fd, write_buf, len) != len) {
            close(fd);
            return;
        }

        short int ret_status = 0;
        short int ret_type = 0;
        long ret_val = 0;
        int read_len = read(fd, buf, LINE_1024);
        if (read_len != -1) {
            ret_status = *((short int *)&buf[0]);
            ret_type = *((short int *)&buf[6]);

        } else {
            close(fd);
            return;
        }
        if (0 == ret_status) {
            if (ret_type < 2) {
                ret_val= *((long int *)&buf[8]);

            } else if (2 == ret_type) {
                float ret_val_float = *((float *)&buf[8]);
                ret_val_float *= 100;
                ret_val = (unsigned long long)ret_val_float;

            } else {
                goto done;
            }
        }
        ((unsigned long long *)&st_ts)[i] = ret_val;
    }
done:
    if (-1 != fd) {
        close(fd);
    }
    pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld",
            st_ts.c_client,
            st_ts.c_server,
            st_ts.c_cache,
            st_ts.c_open,
            st_ts.c_a_client,
            st_ts.t_client,
            st_ts.t_server
             );
    buf[pos] = '\0';
    set_mod_record(mod, buf);
}
*/

/*
void
read_ts_conn_stats(struct module *mod)
{
    int fd = -1;
    int pos = 0;
    struct sockaddr_un un;
    struct stats_ts_conn st_ts;
    int i = 0;
    int record_len = 0;
    char buf[LINE_4096];
    char write_buf[LINE_1024];
    
    bzero(&st_ts, sizeof(st_ts));
    bzero(&un, sizeof(un));
    
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        goto done;
    }
    
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, sock_path);
    if (connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        goto done;
    }

    record_len = sizeof(RECORDS_NAME) / sizeof(RECORDS_NAME[0]);
    
    for (i=0; i < record_len; ++i) {
        int len = 0;
        int info_len = 0;
        int command = 0;
        const char *info = NULL;
        char *pp = NULL;
        
        info = RECORDS_NAME[i];
        info_len = strlen(info);
        command = TS_RECORD_GET;     
        
        memset(write_buf,0,sizeof(write_buf));
        memset(buf,0,sizeof(buf));
        
        pp = write_buf;
        *((int*)pp) = sizeof(command) + 4 + info_len + 1;
        
        pp += 4;
        *((int*)pp) = command;
        
        pp += 4;
        *((int*)pp) = info_len + 1;
        
        pp += 4;
        strcpy(pp,info);
        len = pp - write_buf + info_len + 1;
        
        pp = NULL;
        
        if (write(fd, write_buf, len) != len) {
            close(fd);
            return;
        }
        
        int ret_status = 0;
        int ret_type = 0;
        int64_t ret_val = 0;
        int read_len = 0;
        
        read_len = read(fd, buf, LINE_1024);
        
        if (read_len == -1 || read_len < 20) {
            close(fd);
            return;
        }
        
        pp = buf;
        
        pp += 4;
        ret_status = *((int*)pp); //error code
        
        if (ret_status == 0) {
            pp += 8;
            ret_type = *((int*)pp); //value type
            
            pp += 4;
            pp += (4 + (*((int*)pp)));
            pp += 4;
            
            if (ret_type < 2) {
                ret_val = (*((int64_t*)pp));
            } else if (ret_type == 2) {
                float ret_val_float = (*((float*)pp));
                ret_val_float *= 100;
                ret_val = (int64_t)ret_val_float;
            } else {
                goto done;
            }    
        }
        ((unsigned long long *)&st_ts)[i] = ret_val;
    }

done:
    if (-1 != fd) {
        close(fd);
    }
    
    pos = sprintf(buf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld",
            st_ts.c_client,
            st_ts.c_server,
            st_ts.c_cache,
            st_ts.c_open,
            st_ts.c_a_client,
            st_ts.t_client,
            st_ts.t_server
             );
    buf[pos] = '\0';
    set_mod_record(mod, buf);
}
*/

static int
ts_net_connect(int *fd)
{
    struct sockaddr_un un;
    
    bzero(&un, sizeof(un));
    *fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if (*fd < 0) {
        return -1;
    }
    
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, sock_path);
    if (connect(*fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        return -1;
    }

    return 0;
}

static int
setup_ts_cmd(const char *info, char *cmdbuf)
{
    if (!info || strlen(info) <= 0 || !cmdbuf) {
        return -1;
    }
    
    char *pp = NULL;
    int cmdtype = 0;
    int keylen = 0;
    int cmdlen = 0;
    
    cmdtype = TS_RECORD_GET;
    keylen = strlen(info);
    pp = cmdbuf;
    
    *((int*)pp) = sizeof(cmdtype) + 4 + keylen + 1;
    
    pp += 4;
    *((int*)pp) = cmdtype;
    
    pp += 4;
    *((int*)pp) = keylen + 1;
    
    pp += 4;
    strcpy(pp,info);
    cmdlen = pp - cmdbuf + keylen + 1;
    
    return cmdlen;
}

void
read_ts_conn_stats(struct module *mod)
{
    char sendbuf[LINE_1024];
    char recvbuf[LINE_1024];
    struct stats_ts_conn st_ts;
    int fd = -1;
    int record_size = 0;
    int i = 0;
    int pos = 0;
    
    if (ts_net_connect(&fd) != 0) {
        close(fd);
        return;
    }
    
    int flags;
    if ((flags = fcntl(fd, F_GETFL, 0)) < 0) {
        close(fd);
        return;
    }
    if (fcntl(fd, F_SETFL, flags & ~O_NONBLOCK) < 0) {
        close(fd);
        return;;
    }
    
    struct timeval timeout = {0, 50000};
    
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
    memset(&st_ts,0,sizeof(st_ts));
    record_size = sizeof(RECORDS_NAME) / sizeof(RECORDS_NAME[0]);

    for (i=0; i<record_size; i++) {
        int cmdlen = 0;
        int wrlen = 0;
        int rdlen = 0;
        int fsize = 0;
        
        cmdlen = setup_ts_cmd(RECORDS_NAME[i],sendbuf);
        
        if (cmdlen <= 0) {
            memset(sendbuf,0,sizeof(sendbuf));
            memset(recvbuf,0,sizeof(recvbuf));
            continue;
        }
        
        wrlen = write(fd, sendbuf, cmdlen);
        
        if (wrlen < 0) {
            close (fd);
            return;
            
        } else if (wrlen != cmdlen) {
            close(fd);
            return;            
        }
        
        rdlen = read(fd, recvbuf, sizeof(recvbuf) - 1);
        if (rdlen < 4) {
            close(fd);
            return;
        }
        
        int respdatalen = 0;
        
        respdatalen = *((int*)recvbuf);
        fsize = rdlen;
        
        while((fsize-4)  < respdatalen) {
            rdlen = read(fd, recvbuf + fsize, sizeof(recvbuf) - (fsize) - 1);
            fsize += rdlen;
        }

        recvbuf[fsize+4] = '\0';
        
        if (fsize < 4) {
            close(fd);
            return;
        }
        
        char *pp = NULL;
        int ret_status = 0;
        int ret_type = 0;
        int64_t ret_val = 0;
        
        pp = recvbuf;
	pp += 16;
        pp += 4;
        pp += strlen(RECORDS_NAME[i]) + 1;
        pp += 4;
        ret_val = (*((int64_t*)pp));
    /*    
        pp += 4;
        ret_status = *((int*)pp); //error code
        
        if (ret_status == 0) {
            pp += 8;
            ret_type = *((int*)pp); //value type
            
            pp += 4;
            pp += (4 + (*((int*)pp)));
            pp += 4;
            
            if (ret_type < 2) {
                ret_val = (*((int64_t*)pp));
            } else if (ret_type == 2) {
                float ret_val_float = (*((float*)pp));
                ret_val_float *= 100;
                ret_val = (int64_t)ret_val_float;
            } else {
                goto done;
            }    
        }
*/
        ((unsigned long long *)&st_ts)[i] = ret_val;
        memset(sendbuf,0,sizeof(sendbuf));
        memset(recvbuf,0,sizeof(recvbuf));
    }

done:

    if (fd != -1) {
        close(fd);
    }
    
    pos = sprintf(recvbuf, "%lld,%lld,%lld,%lld,%lld,%lld,%lld",
            st_ts.c_client,
            st_ts.c_server,
            st_ts.c_cache,
            st_ts.c_open,
            st_ts.c_a_client,
            st_ts.t_client,
            st_ts.t_server
             );
    recvbuf[pos] = '\0';
    set_mod_record(mod, recvbuf);
}

void
mod_register(struct module *mod)
{
    register_mod_fields(mod, "--ts_conn", ts_conn_usage, ts_conn_info, 7, read_ts_conn_stats, set_ts_conn_record);
}
