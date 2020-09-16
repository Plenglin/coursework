#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <poll.h>

#define FORK_N 4

/**
 * Creates n processes. Returns the index, where -1 is the parent.
 */
int fork_n(int *pids, int n) {
    for (int i = 0; i < n; i++) {
        int pid = fork();
        if (pid == 0) {
            return i;
        } else {
            pids[i] = pid;
        }
    }
    return -1;
}

const char * const SENTENCES[] = {
    "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. ",
    "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues.",
    "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. "
};
const int SENTENCES_COUNT = sizeof(SENTENCES) / sizeof(char*);

int child_pids[FORK_N];
char *buf;

void handle_sigint(int sig) { 
    char buf[1000];
    sprintf(buf, "signal %d\n", sig); 
    write(0, buf, strlen(buf));
    for (int i = 0; i < FORK_N; i++) {
        kill(child_pids[i], SIGKILL);
    }
    wait(0);
    munmap(buf, 8192);
    exit(0);
} 

template <class Lock>
int run_mutex_test() {
    printf("parent %d\n", getpid());
    signal(SIGINT, handle_sigint); 
    
    buf = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buf[0] = 0;

    Lock mutex(FORK_N);
    int index = fork_n(child_pids, FORK_N);
    if (index == -1) {  // parent
        while (1) sleep(1);
    }
    mutex.set_i(index);

    for (unsigned long i = 0;; i++) {
        auto _lock = mutex.lock();
        if (index % 2 == 0) {
            write(0, buf, strlen(buf));
            write(0, "\n", 1);
        } else {
            strcpy(buf, SENTENCES[(i + index) % SENTENCES_COUNT]);
        }
    }
    return 0; 
}
