#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <poll.h>

#define FORK_N 1

int parent_pid;
int *kill_pids;
int n_kill_pids;

/**
 * Creates n processes. Returns the index, where 0 is the parent.
 */
int fork_n(int n) {
    n_kill_pids = n - 1;
    kill_pids = (int*)malloc(n_kill_pids * sizeof(int));
    parent_pid = getpid();
    for (int i = 1; i < n; i++) {
        int pid = fork();
        if (pid == 0) {
            return i;
        } else {
            kill_pids[i - 1] = pid;
        }
    }
    return 0;
}

int __index = -1;
int kill_pipe_fd;

void isr(int sig) {
    char buf[1000];
    int len = sprintf(buf, "int %d\n", __index);
    char b = 'k';
    write(0, buf, len);
    close(kill_pipe_fd);
}

const char * const SENTENCES[] = {
    "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. ",
    "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues.",
    "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. "
};
const int SENTENCES_COUNT = sizeof(SENTENCES) / sizeof(char*);

template <class Lock>
int run_mutex_test() {
    auto mutex = Lock(FORK_N);
    struct pollfd killpollfd;
    {
        int fds[2];
        pipe(fds);
        killpollfd.fd = fds[0];
        kill_pipe_fd = fds[1];
    }
    
    char *buf = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buf[0] = 0;

    int index = fork_n(FORK_N);
    __index = index;
    signal(SIGINT, isr);
    mutex.set_i(index);

    for (unsigned long i = 0;; i++) {
        auto _lock = mutex.lock();
        if (index % 2 == 0) {
            write(0, buf, strlen(buf));
            write(0, "\n", 1);
        } else {
            strcpy(buf, SENTENCES[(i + index) % SENTENCES_COUNT]);
        }
        if (__index == 0) {
            int ret = poll(&killpollfd, 1, 0);
            if (killpollfd.events != 0) {
                break;
            }
        }
    }

    printf("%d stopping\n", index);
    fflush(0);
    if (index != 0) {
        exit(0);
    } else {
        for (int i = 0; i < n_kill_pids; i++) {
            kill(kill_pids[i], SIGTERM);
        }
    }
    wait(0);
    munmap(buf, 8192);
    free(kill_pids);
}
