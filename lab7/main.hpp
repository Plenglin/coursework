#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#define FORK_N 4

int *kill_pids;
int n_kill_pids;

/**
 * Creates n processes. Returns the index, where 0 is the parent.
 */
int fork_n(int n) {
    n_kill_pids = n;
    kill_pids = (int*)malloc(n_kill_pids * sizeof(int));
    kill_pids[0] = getpid();
    for (int i = 1; i < n; i++) {
        int pid = fork();
        if (pid == 0) {
            return i;
        } else {
            kill_pids[i] = pid;
        }
    }
    return 0;
}

volatile sig_atomic_t should_loop;
int __index = -1;

void isr(int sig) {
    printf("interrupted\n");
    if (__index == 0) {  // parent
        should_loop = false;
        for (int i = 1; i < n_kill_pids; i++) {
            printf("killing %d\n", i);
            kill(kill_pids[i], SIGKILL);
        }
    } else {
        printf("notifying parent\n");
        kill(kill_pids[0], SIGINT);
    }
}

char *SENTENCES[] = {
    "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. ",
    "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues.",
    "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. "
};
const int SENTENCES_COUNT = sizeof(SENTENCES) / sizeof(char*);

template <class Lock>
int run_mutex_test() {
    auto mutex = Lock(FORK_N);
    
    char *buf = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buf[0] = 0;

    int index = fork_n(FORK_N);
    __index = index;
    mutex.set_i((int)index);
    should_loop = true;
    signal(SIGINT, isr);

    for (unsigned long i = 0;; i++) {
        auto _lock = mutex.lock();
        if (index % 2 == 0) {
            printf("%ld: %s\n", i, buf);
        } else {
            strcpy(buf, SENTENCES[(i + index) % SENTENCES_COUNT]);
        }
        if (!should_loop) {
            break;
        }
    }

    printf("%d stopping\n", index);
    if (index != 0) exit(0);
    wait(0);
    munmap(buf, 8192);
    free(kill_pids);
}
