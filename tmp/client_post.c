#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SERV_IP "0.0.0.0"
#define HOST "1212.com"
#define SERV_PORT 80
#define TMP_FILE "./post.file"

char pre_request_header[8192] = "POST /path HTTP/1.1\r\nUser-Agent: Mozilla/5.0 \r\nContent-Type: application/json;charset=UTF-8\r\nConnection: keep-alive\r\nAccept-Language: zh-CN,zh;q=0.9,en;q=0.8\r\nAccept: application/json, text/plain, */*\r\n";

int file_size(const char* filePath) {
    if(filePath == NULL) return 0;
    struct stat sb;
    if(stat(filePath, &sb) < 0)
    return 0;
    return sb.st_size;
}

int main(void)
{
    int sfd;
    struct sockaddr_in serv_addr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(SERV_PORT);

    connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    if (1) {
        int size = file_size(TMP_FILE);
        if (size <= 0) {
            return 0;
        }
        char request_header[8192] = {0};
        sprintf(request_header, "%sHost: %s\r\nContent-Length: %d\r\n\r\n", 
                pre_request_header, HOST, size);
        int ret = write(sfd, request_header, strlen(request_header));
        printf("%s", request_header);

        FILE *f = fopen(TMP_FILE, "r");
        if (f == NULL) { 
            return 0;
        }

        while (size) {
            char buffer[1024] = {0};
            int ret = fread(&buffer[0], 1, 1024, f);
            size -= ret;

            ret = write(sfd, buffer, ret);
            if (ret <= 0) {
                break;
            }
            printf("%s", buffer);
        }
        printf("\nsend body done: remain size: %d\n", size);
    }

    char buf[1024] = {0};
    read(sfd, buf, sizeof(buf));
    printf("buf: %s\n", buf);
    
    printf("req done\n");
    close(sfd);
    return 0;
}

