#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

static void daemon_child_handle(int signal) {
    printf("daemon_child_handle\n");
    return;
}

static void daemon_stop_handle(int signal) {
    printf("daemon_stop_handle\n");
    return;
}

int main(void) {
    printf("mpid = %d, main_parent_pid = %d\n", getpid(), getppid());

    signal(SIGCHLD, daemon_child_handle);
    signal(SIGTERM, daemon_stop_handle);
    // signal(SIGINT, daemon_stop_handle);
    signal(SIGQUIT, daemon_stop_handle);
    signal(SIGKILL, daemon_stop_handle);

    pid_t pid;
    pid = fork();
    if (pid > 0) {
        while (1) {
            printf("parent_pid = %d, parent_parent_id = %d\n", getpid(), getppid());
            sleep(5);
        }
    } else if (pid == 0) {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGTERM);
        //if (sigprocmask(SIG_SETMASK, &set, NULL) == -1) {
        if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
            printf("set child mask failed\n");
        }
        //signal(SIGTERM, SIG_IGN);
        while (1) {
            printf("child_pid = %d, child_parent_id = %d\n", getpid(), getppid());
            sleep(5);
        }
    }
    return 0;
}

