#include "hloop.h"
#include "hsocket.h"
#include <udns.h>
#include <event/hevent.h>

static hloop_t* hloop = NULL;

struct resolve_query_t;
typedef void resolve_cb(struct sockaddr_storage *storage,
        struct resolve_query_t *query);
struct resolve_query_t {
    struct dns_query *q;
    resolve_cb *cb;
    void *data;
};

static struct dns_ctx *g_ctx = &dns_defctx;
static hio_t *g_resolve_io_watcher;
static htimer_t *g_resolve_timeout_watcher;

static void init_resolver() {
    //g_resolve_io_watcher = malloc(sizeof(hio_t));
    //g_resolve_timeout_watcher = malloc(sizeof(htimer_t));
}

static void resolve_sock_cb(hloop_t *loop, hio_t *w, int revents) {
    struct dns_ctx *ctx = (struct dns_ctx *)(hio_context(w));
    if (revents & HV_READ) {
        dns_ioevent(ctx, time(NULL));
    }
}

static void resolve_timeout_cb(hloop_t *loop, struct ev_timer *w, int revents) {
    int next = dns_timeouts(NULL, -1, time(NULL));
    /*
    if (next > 0) {
        w->repeat = next;
        ev_timer_again (EV_A_ w);
    } else {
        w->repeat = 1;
        ev_timer_again (EV_A_ w);
    }
     */
}

static void dns_timer_setup_cb(struct dns_ctx *ctx, int timeout, void *data) {
    struct ev_loop *loop = (struct ev_loop *)data;
    /*
    if (ev_is_active(&g_resolve_timeout_watcher)) {
        ev_timer_stop(loop, &g_resolve_timeout_watcher);
    }
    if (ctx != NULL && timeout >= 0) {
        ev_timer_set(&g_resolve_timeout_watcher, timeout, 0.0);
        ev_timer_start(loop, &g_resolve_timeout_watcher);
    }
     */
}

int resolve_init(hloop_t *loop, char **nameservers, int nameserver_num) {
    int i;
    if (NULL == nameservers) {
        dns_reset(g_ctx);
        dns_init(g_ctx, 0);
    } else {
        dns_reset(g_ctx);
        for (i = 0; i < nameserver_num; i++) {
            dns_add_serv(g_ctx, nameservers[i]);
        }
    }

    int sockfd = dns_open(g_ctx);
    if (sockfd < 0) {
        return -1;
    }
    if (nonblocking(sockfd) < 0) {
        close(sockfd);
        return -1;
    }

    g_resolve_io_watcher = hio_get(loop, sockfd);
    hio_init(g_resolve_io_watcher);
    hio_setcb_read(g_resolve_io_watcher, resolve_sock_cb);
    hio_set_context(g_resolve_io_watcher, (void*)g_ctx);

    //ev_timer_init(&g_resolve_timeout_watcher, resolve_timeout_cb, 0, 0.0);
    //g_resolve_timeout_watcher.data = g_ctx;
    //ev_timer_start(loop, &g_resolve_timeout_watcher);
    // dns_set_tmcbck(g_ctx, dns_timer_setup_cb, loop);
    return sockfd;
}

void resolve_shutdown(struct ev_loop *loop) {
    /*
    ev_io_stop(loop, &g_resolve_io_watcher);
    if (ev_is_active(&g_resolve_timeout_watcher)) {
        ev_timer_stop(loop, &g_resolve_timeout_watcher);
    }
    dns_close(g_ctx);
     */
}

void resolve_cancel(struct resolve_query_t *query) {
    dns_cancel(g_ctx, query->q);
    free(query);
}

static void dns_query_v4_cb(struct dns_ctx *ctx, struct dns_rr_a4 *result, void *data) {
    struct resolve_query_t *query = (struct resolve_query_t *)data;
    if (NULL == result) {
        query->cb(NULL, query);
        return;
    }

    if (result->dnsa4_nrr > 0) {
        struct sockaddr_storage storage;
        storage.ss_family = AF_INET;
        struct sockaddr_in *addr = (struct sockaddr_in *)&storage;
        addr->sin_addr = result->dnsa4_addr[0];
        query->cb(&storage, query);
    }
    free(result);
}

struct resolve_query_t *resolve_query(char *hostname, resolve_cb *cb, void *data) {
    struct resolve_query_t *query = (struct resolve_query_t *)malloc(sizeof(struct resolve_query_t));
    if (NULL == query) {
        return NULL;
    }
    memset(query, 0, sizeof(struct resolve_query_t));
    query->data = data;
    query->cb = cb;
    query->q = dns_submit_a4(g_ctx, hostname, 0, dns_query_v4_cb, query);
    if (NULL == query->q) {
        free(query);
        return NULL;
    }
    return query;
}

int strtosockaddr(const char *src, void *addrptr) {
    int ret;
    struct sockaddr_storage *storage = (struct sockaddr_storage *)addrptr;
    struct sockaddr_in addr4;
    ret = inet_pton(AF_INET, src, &(addr4.sin_addr));
    if (ret > 0) {
        storage->ss_family = AF_INET;
        struct sockaddr_in *addr = (struct sockaddr_in *)addrptr;
        memcpy(&addr->sin_addr, &addr4.sin_addr, sizeof(addr4.sin_addr));
        return ret;
    }

    struct sockaddr_in6 addr6;
    ret = inet_pton(AF_INET6, src, &(addr6.sin6_addr));
    if (ret > 0) {
        storage->ss_family = AF_INET6;
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)addrptr;
        memcpy(&addr->sin6_addr, &addr6.sin6_addr, sizeof(addr6.sin6_addr));
        return ret;
    }
    return -1;
}

#define BUFFER_MAX_PREALLOC (1024*1024)
#define BUFFER_DEFAULT_SIZE 2048
#define SOCKS5_DEFAULT_BUFFER_SIZE 128

#define SOCKS5_CONN_STAGE_EXMETHOD          1
#define SOCKS5_CONN_STAGE_USERNAMEPASSWORD  2
#define SOCKS5_CONN_STAGE_EXHOST            3
#define SOCKS5_CONN_STAGE_DNSQUERY          4
#define SOCKS5_CONN_STAGE_CONNECTING        5
#define SOCKS5_CONN_STAGE_CONNECTED         6
#define SOCKS5_CONN_STAGE_STREAM            7
#define SOCKS5_CONN_STAGE_CLOSING           8
#define SOCKS5_CONN_STAGE_CLOSED            9

typedef struct {
    size_t size;
    size_t len;
    char *data;
} buffer_t;

buffer_t *buffer_new(size_t size) {
    buffer_t *buf = (buffer_t *)malloc(sizeof(buffer_t));
    if (NULL == buf) {
        return NULL;
    }
    buf->size = size;
    buf->len = 0;
    buf->data = NULL;
    if (size > 0) {
        buf->data = (char *)malloc(size);
        if (NULL == buf->data) {
            free(buf);
            return NULL;
        }
    }
    return buf;
}

void buffer_free(buffer_t *buf) {
    free(buf->data);
    free(buf);
}

size_t buffer_len(buffer_t *buf) {
    return buf->len;
}

size_t buffer_size(buffer_t *buf) {
    return buf->size;
}

static buffer_t *buffer_trymakeroom(buffer_t *buf, size_t addsize) {
    size_t newsize = buffer_len(buf) + addsize;
    if (buffer_size(buf) >= newsize) {
        return buf;
    }
    if (newsize < BUFFER_MAX_PREALLOC) {
        newsize *= 2;
    } else {
        newsize += BUFFER_MAX_PREALLOC;
    }
    char *data = (char *)realloc(buf->data, newsize);
    if (NULL == data) {
        return NULL;
    }
    buf->data = data;
    buf->size = newsize;
    return buf;
}

buffer_t *buffer_concat(buffer_t *buf, const char *data, size_t len) {
    buf = buffer_trymakeroom(buf, len);
    if (NULL == buf) {
        return NULL;
    }
    memcpy(buf->data + buf->len, data, len);
    buf->len += len;
    return buf;
}

buffer_t *buffer_slice(buffer_t *src, size_t start, size_t end) {
    size_t size = end - start;
    buffer_t *buf = buffer_new(size);
    if (NULL == buf) {
        return NULL;
    }
    return buffer_concat(buf, src->data + start, size);
}

buffer_t *buffer_reset(buffer_t *buf) {
    free(buf->data);
    buf->data = NULL;
    buf->size = 0;
    buf->len = 0;
    return buf;
}

struct ss5_client_conn {
    int         fd;
    hio_t       *rwio;
    buffer_t    *input;
    buffer_t    *output;
};

struct ss5_proxy_config {
};

struct ss5_server_conn {
    int         fd;
    hio_t       *rwio;
    buffer_t    *input;
    buffer_t    *output;
    char        hostname[512];
    uint16_t    port;
    uint8_t     addrtype;
    buffer_t    *bndaddr;
    struct resolve_query_t *query;
};

struct ss5_conn {
    uint8_t                 stage;
    struct ss5_client_conn  client;
    struct ss5_server_conn  server;
};

static char proxy_host[64] = "127.0.0.1";
static int  proxy_port = 80;
static int  proxy_ssl = 0;

void ss5_conn_close(struct ss5_conn *conn) {
    conn->stage = SOCKS5_CONN_STAGE_CLOSED;
    /*
    if (conn->client.fd) ev_io_stop(loop, conn->client.rw);
    if (conn->client.fd) ev_io_stop(loop, conn->client.ww);
     */

    if (conn->client.input) {
        buffer_free(conn->client.input);
    }
    if (conn->client.output) {
        buffer_free(conn->client.output);
    }
    if (conn->client.fd) {
        close(conn->client.fd);
    }
    free(conn);
}

void ss5_conn_setstage(struct ss5_conn *conn, uint8_t stage) {
    conn->stage = stage;
}

struct ss5_conn *socks5_conn_new() {
    struct ss5_conn *conn = (struct ss5_conn *)malloc(sizeof(struct ss5_conn));
    if (NULL == conn) {
        goto _clean;
    }
    memset(conn, 0, sizeof(struct ss5_conn));
    conn->stage = SOCKS5_CONN_STAGE_EXMETHOD;

    conn->client.input = buffer_new(SOCKS5_DEFAULT_BUFFER_SIZE);
    if (NULL == conn->client.input) {
        goto _clean;
    }
    conn->client.output = buffer_new(SOCKS5_DEFAULT_BUFFER_SIZE);
    if (NULL == conn->client.output) {
        goto _clean;
    }

    conn->server.input = buffer_new(SOCKS5_DEFAULT_BUFFER_SIZE);
    if (NULL == conn->server.input) {
        goto _clean;
    }
    conn->server.output = buffer_new(SOCKS5_DEFAULT_BUFFER_SIZE);
    if (NULL == conn->server.output) {
        goto _clean;
    }
    conn->server.bndaddr = buffer_new(128);
    if (NULL == conn->server.bndaddr) {
        goto _clean;
    }

    return conn;
    _clean:
    if (conn) {
        ss5_conn_close(conn);
    }
    return NULL;
}

// hloop_create_tcp_server -> on_accept -> hio_setup_tcp_upstream

static void on_client_close(hio_t* io) {
    printf("on_close fd=%d error=%d\n", hio_fd(io), hio_error(io));
}


struct ss5_method_req {
    uint8_t ver;
    uint8_t nmethods;
    uint8_t methods[0];
};

struct ss5_method_res {
    uint8_t ver;
    uint8_t method;
};

struct ss5_request {
    uint8_t ver;
    uint8_t cmd;
    uint8_t rsv;
    uint8_t addrtype;
};

#define SOCKS5_RSV 0x00
#define SOCKS5_ADDRTYPE_IPV4 0x01
#define SOCKS5_ADDRTYPE_DOMAIN 0x03
#define SOCKS5_ADDRTYPE_IPV6 0x04

#define SOCKS5_RESPONSE_SUCCESS 0x00
#define SOCKS5_RESPONSE_SERVER_FAILURE 0x01
#define SOCKS5_RESPONSE_CONNECTION_NOT_ALLOWED 0x02
#define SOCKS5_RESPONSE_NETWORK_UNREACHABLE 0x03
#define SOCKS5_RESPONSE_HOST_UNREACHABLE 0x04
#define SOCKS5_RESPONSE_CONNECTION_REFUSED 0x05
#define SOCKS5_RESPONSE_TTL_EXPIRED 0x06
#define SOCKS5_RESPONSE_COMMAND_NOT_SUPPORTED 0x07
#define SOCKS5_RESPONSE_ADDRTYPE_NOT_SUPPORTED 0x08
struct ss5_response {
    uint8_t ver;
    uint8_t rep;
    uint8_t rsv;
    uint8_t addrtype;
};

#define SOCKS5_VERSION 0x05
#define SOCKS5_AUTH_NOAUTH 0x00
#define SOCKS5_AUTH_USERNAMEPASSWORD 0x02
#define SOCKS5_AUTH_NOACCEPTABLE 0xff

static void on_server_send(hio_t* io, void* buf, int readbytes) {
    struct ss5_conn         *conn = NULL;
    struct ss5_client_conn  *client = NULL;

    conn = (struct ss5_conn *)hio_context(io);
    client = &conn->client;
    ss5_conn_setstage(conn, SOCKS5_CONN_STAGE_CONNECTED);
    // reset io
    struct ss5_response reply = {
            SOCKS5_VERSION,
            SOCKS5_RESPONSE_SUCCESS,
            SOCKS5_RSV,
            SOCKS5_ADDRTYPE_IPV4
    };
    hio_write(client->rwio, (char*)&reply, sizeof(reply));
    char zero_str[6] = {0};
    hio_write(client->rwio, zero_str, sizeof(zero_str));
    hio_setup_upstream(conn->client.rwio, conn->server.rwio);
    hio_read_upstream(io);
    return;
}

static void on_client_send(hio_t* io, void* buf, int readbytes) {
    struct ss5_conn         *conn = NULL;
    struct ss5_client_conn  *client = NULL;

    conn = (struct ss5_conn *)hio_context(io);
    if (conn == NULL) {
        goto failed;
    }
    client = &conn->client;
    switch (conn->stage) {
        case SOCKS5_CONN_STAGE_EXMETHOD : {
            ss5_conn_setstage(conn, SOCKS5_CONN_STAGE_EXHOST);
            // monitor read
            break;
        }
        default: {
            break;
        }
    }

end:
    return;
failed:
    ss5_conn_close(conn);
    return;
}

void dns_resolve_cb(struct sockaddr_storage *storage, struct resolve_query_t *query) {
    struct ss5_conn *conn = (struct ss5_conn *)query->data;
    struct ss5_server_conn *server = &conn->server;
    struct ss5_client_conn *client = &conn->client;
    socklen_t address_len;
    char ipaddr[INET6_ADDRSTRLEN];
    struct ss5_response reply = {
            SOCKS5_VERSION,
            SOCKS5_RESPONSE_SERVER_FAILURE,
            SOCKS5_RSV,
            SOCKS5_ADDRTYPE_IPV4
    };
    if (NULL == storage) {
        goto _err;
    }

    if (AF_INET == storage->ss_family) {
        struct sockaddr_in *addr = (struct sockaddr_in *)storage;
        address_len = sizeof(struct sockaddr_in);
        if (NULL == inet_ntop(storage->ss_family, &(addr->sin_addr), ipaddr, sizeof(ipaddr))) {
            goto _err;
        }
        addr->sin_port = htons(server->port);
    } else if (AF_INET6 == storage->ss_family) {
        struct sockaddr_in6 *addr = (struct sockaddr_in6 *)storage;
        address_len = sizeof(struct sockaddr_in6);
        if (NULL == inet_ntop(storage->ss_family, &(addr->sin6_addr), ipaddr, sizeof(ipaddr))) {
            goto _err;
        }
        addr->sin6_port = htons(server->port);
    } else {
        goto _err;
    }

    if ('\0' == server->hostname[0]) {
        memcpy(server->hostname, ipaddr, sizeof(ipaddr));
    } else {
        printf("connecting to remote host=%s(%s), port=%d\n", server->hostname, ipaddr, server->port);
    }

    server->rwio = hio_setup_tcp_upstream(client->rwio, server->hostname, server->port, 1);
    ss5_conn_setstage(conn, SOCKS5_CONN_STAGE_CONNECTING);
    return;
    _err:
    buffer_concat(client->output, (char *)&reply, sizeof(reply));
    ss5_conn_setstage(conn, SOCKS5_CONN_STAGE_CLOSING);
    //ev_io_start(loop, client->ww);
}

static void on_client_recv(hio_t* io, void* buf, int readbytes) {
    /*
    hio_write(io, buf, readbytes);
    hio_read_stop(io);
    hio_read_once(io);
    hio_read_until(io, TEST_READ_UNTIL);
     */
    struct ss5_conn         *conn = NULL;
    struct ss5_client_conn  *client = NULL;
    struct ss5_server_conn  *server = NULL;

    conn = (struct ss5_conn *)hio_context(io);
    if (conn == NULL) {
        goto failed;
    }
    client = &conn->client;
    server = &conn->server;
    buffer_concat(conn->client.input, buf, readbytes);

    switch (conn->stage) {
        case SOCKS5_CONN_STAGE_EXMETHOD: {
            struct ss5_method_req *method_req = NULL;
            method_req = (struct ss5_method_req *)client->input->data;
            if (SOCKS5_VERSION != method_req->ver) {
                printf("version: %d not support\n", method_req->ver);
                goto failed;
            }
            if (buffer_len(client->input) < (method_req->nmethods + 2)) {
                // monitor read again
                return;
            }
            // only support no auth
            struct ss5_method_res reply = {
                SOCKS5_VERSION,
                SOCKS5_AUTH_NOAUTH
            };
            buffer_concat(client->output, (char*)&reply, sizeof(reply));
            buffer_reset(client->input);
            // monitor write
            hio_write(io, client->output->data, client->output->len);
            break;
        }
        case SOCKS5_CONN_STAGE_EXHOST : {
            struct ss5_request *req = (struct ss5_request*)client->input->data;
            if (SOCKS5_VERSION != req->ver) {
                goto failed;
            }
            if (buffer_len(client->input) < sizeof(struct ss5_request)) {
                return;
            }
            struct ss5_response reply = {
                SOCKS5_VERSION,
                SOCKS5_RESPONSE_SUCCESS,
                SOCKS5_RSV,
                SOCKS5_ADDRTYPE_IPV4
            };
            struct sockaddr_storage storage;
            memset(&storage, 0, sizeof(struct sockaddr_storage));
            switch (req->addrtype) {
                case SOCKS5_ADDRTYPE_IPV4 : {
                    if (buffer_len(client->input) < sizeof(struct ss5_request) + 6) {
                        // monitor read
                        return;
                    }
                    struct sockaddr_in *addr = (struct sockaddr_in*)&storage;
                    addr->sin_family = AF_INET;
                    char *ipv4 = client->input->data + sizeof(struct ss5_request);
                    char *port = ipv4 + 4;
                    memcpy(&addr->sin_addr.s_addr, ipv4, 4);
                    memcpy(&addr->sin_port, port, 2);

                    char ip_str[16] = {0};
                    inet_ntop(storage.ss_family, &(addr->sin_addr), ip_str, sizeof(ip_str));
                    memcpy(server->hostname, ip_str, sizeof(ip_str));
                    printf("server->hostname: %s\n", server->hostname);

                    server->port = ntohs(addr->sin_port);
                    buffer_concat(server->bndaddr, (char*)&addr->sin_addr.s_addr, 4);
                    buffer_concat(server->bndaddr, (char*)&addr->sin_port, 2);
                    break;
                }
                case SOCKS5_ADDRTYPE_DOMAIN : {
                    if (buffer_len(client->input) < sizeof(struct ss5_request) + 1) {
                        // monitro read
                        return;
                    }
                    int hostname_len = *(client->input->data + sizeof(struct ss5_request));
                    if (buffer_len(client->input) < (sizeof(struct ss5_request) + hostname_len + 3)) {
                        // monitro read
                        return;
                    }
                    memcpy(server->hostname, client->input->data + sizeof(struct ss5_request) + 1, hostname_len);
                    char *port = client->input->data + sizeof(struct ss5_request) + 1 + hostname_len;
                    uint16_t sin_port;
                    memcpy(&sin_port, port, 2);
                    server->port = ntohl(sin_port);

                    if (strtosockaddr(server->hostname, (void*)&storage) > 0) {
                        if (storage.ss_family == AF_INET) {
                            server->addrtype = SOCKS5_ADDRTYPE_IPV4;
                            struct sockaddr_in *addr = (struct sockaddr_in*)&storage;
                            addr->sin_port = htons(server->port);
                            buffer_concat(server->bndaddr, (char*)&addr->sin_addr.s_addr, 4);
                            buffer_concat(server->bndaddr, (char*)&addr->sin_port, 2);
                        } else if (storage.ss_family == AF_INET6) {
                            server->addrtype = SOCKS5_ADDRTYPE_IPV6;
                            struct sockaddr_in6 *addr = (struct sockaddr_in6*)&storage;
                            addr->sin6_port = htons(server->port);
                            buffer_concat(server->bndaddr, (char*)&addr->sin6_addr, 16);
                            buffer_concat(server->bndaddr, (char*)&addr->sin6_port, 2);
                        }
                        break;
                    } else {
                        buffer_concat(server->bndaddr, client->input->data + sizeof(struct ss5_request), hostname_len + 3);
                        server->query = resolve_query(server->hostname, dns_resolve_cb, conn);
                        return;
                    }
                }
                default: {
                    goto failed;
                    break;
                }
            }

            buffer_reset(client->input);
            server->rwio = hio_setup_tcp_upstream(client->rwio, server->hostname, server->port,
                    server->port == 80 ? 0 : 1);
            hio_set_context(server->rwio, (void*)conn);
            printf("upstream io: %x\n", server->rwio);
            hio_setcb_connect(server->rwio, on_server_send);
            ss5_conn_setstage(conn, SOCKS5_CONN_STAGE_CONNECTING);
            break;
        }
        default: {
            break;
        }
    }
end:
    return;
failed:
    ss5_conn_close(conn);
    return;
}

static void on_client_accept(hio_t* io) {
    //hio_setup_tcp_upstream(io, proxy_host, proxy_port, proxy_ssl);
    struct ss5_conn         *conn = NULL;
    printf("on_accept connfd=%d\n", hio_fd(io));
    char localaddrstr[SOCKADDR_STRLEN] = {0};
    char peeraddrstr[SOCKADDR_STRLEN] = {0};
    printf("accept connfd=%d [%s] <= [%s]\n", hio_fd(io),
           SOCKADDR_STR(hio_localaddr(io), localaddrstr),
           SOCKADDR_STR(hio_peeraddr(io), peeraddrstr));

    conn = socks5_conn_new();
    if (conn == NULL) {
        goto failed;
    }
    conn->client.rwio = io;
    printf("client io: %x\n", io);
    hio_set_context(io, (void*)conn);
    hio_setcb_close(io, on_client_close);
    hio_setcb_read(io, on_client_recv);
    hio_setcb_write(io, on_client_send);

    hio_read_start(io);
    return;
failed:
    hio_close(io);
    return;
}

int main(int argc, char** argv) {
    /*
    if (argc < 3) {
        printf("Usage: %s port proxy_host:proxy_port\n", argv[0]);
        return -10;
    }
    int port = atoi(argv[1]);
    char* pos = strchr(argv[2], ':');
    if (pos) {
        int len = pos - argv[2];
        if (len > 0) {
            memcpy(proxy_host, argv[2], len);
            proxy_host[len] = '\0';
        }
        proxy_port = atoi(pos + 1);
    } else {
        strncpy(proxy_host, argv[2], sizeof(proxy_host));
    }
     */
    int port = 1967;
    if (proxy_port == 0) proxy_port = 2048;
    printf("proxy: [%s:%d]\n", proxy_host, proxy_port);
    init_resolver();
    hloop = hloop_new(0);
    hio_t* listenio = hloop_create_tcp_server(hloop, "0.0.0.0", port, on_client_accept);
    if (listenio == NULL) {
        return -20;
    }
    printf("listenfd=%d\n", hio_fd(listenio));
    hloop_run(hloop);
    hloop_free(&hloop);
    return 0;
}
