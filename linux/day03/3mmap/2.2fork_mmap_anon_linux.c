#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(void) {
    int *p;
    pid_t pid;

    p = (int*)mmap(NULL, 4, PROT_READ|PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);  //MAP_ANONYMOUS
    if(p == MAP_FAILED) {
        perror("mmap error");
        exit(1);
    }
    pid = fork();
    if(pid == 0) {
        *p = 2000;
        printf("child, *p = %d\n", *p);
    } else {
        sleep(1);
        printf("parent, *p = %d\n", *p);
    }
    munmap(p, 4);
    return 0;
}

