#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


void interrupt(int sig) {
    printf("caught signal %d haha no\n", sig);
}

int main() {
    int parent_pid = getpid();
    int child_pid = fork();
    if (child_pid) {
        printf("Parent: %d; Child: %d\n", parent_pid, child_pid);
    }

    // Signal handler on ALL the signals
    for (int i = 1; i < 30; i++) {
        signal(i, interrupt);
    }

    printf("\033[3mWhile you were trying to end my process, I studied the blade\033[0m\n");

    while (1) {
        sleep(1);
    }
}