#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "child.h"

#define SLEEP_SECONDS 1  // TODO change this

int main() {
    while (1) {
        int parent_pid = getpid();
        int child_pid = fork();
        if (child_pid) {
            printf("Parent: %d; Child: %d\n", parent_pid, child_pid);
        } else {
            do_child(parent_pid);
            exit(0);
        }
        wait(0);
        printf("Child process terminated. Sleeping 10 seconds and relaunching.\n");
        return 0;
        sleep(SLEEP_SECONDS);
    }
    return 0;
}