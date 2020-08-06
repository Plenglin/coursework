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

void handle_sigint(int sig) 
{ 
    printf("Caught signal %d\n", sig); 
} 

const char * const SENTENCES[] = {
    "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. ",
    "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues.",
    "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. "
};
const int SENTENCES_COUNT = sizeof(SENTENCES) / sizeof(char*);

template <class Lock>
int run_mutex_test() {
    printf("parent %d\n", getpid());
    signal(SIGINT, handle_sigint); 
    while (1) ; 
    return 0; 
}
