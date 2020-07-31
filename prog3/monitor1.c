#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "child.h"


void do_parent(int child_pid) {
    while (1) {


        sleep(10);
    }
}

int main() {
    int parent_pid = getpid();
    int child_pid = fork();
    if (child_pid) {
        printf("Parent: %d; Child: %d\n", parent_pid, child_pid);
        do_parent(child_pid);
    } else {
        do_child(parent_pid);
    }

    return 0;
}