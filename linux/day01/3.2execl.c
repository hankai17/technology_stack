#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(void) {
    pid_t pid;
    pid = fork();
    if (pid == -1 ) {
        perror("fork");
        exit(1);
    } else if (pid > 0) {
        wait(NULL); //父进程回收资源
        printf("parent pid = %d, parentID = %d\n", getpid(), getppid());
    } else if (pid == 0) {
        sleep(3);
        printf("i am child\n");
        execl("/bin/ls", "ls", "-l",  NULL);
        perror("exec");
        exit(1);
    }
    printf("-------finish...%d\n", getpid());
    return 0;
}

