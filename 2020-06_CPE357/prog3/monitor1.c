#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "child.h"


void parent_isr(int sig) {
    // noop
}

int do_parent(int child_pid) {
    __message->status = STATUS_UNKNOWN;
    sleep(1);
    while (1) {
        kill(child_pid, SIGUSR1);
        sleep(10);  
        switch (__message->status) {
            case STATUS_QUIT:
                return 0;
            case STATUS_UNKNOWN:
                return 1;
            case STATUS_ALIVE: 
                break;
        }
        // Reset status
        __message->status = STATUS_UNKNOWN;
    }
}

void monitor_loop() {
    while (1) {
        int parent_pid = getpid();
        int child_pid = fork();
        if (child_pid) {
            printf("Parent: %d; Child: %d\n", parent_pid, child_pid);
            int should_restart = do_parent(child_pid);
            if (!should_restart) {
                printf("Child quit. Halting.\n");
                return;
            }
            printf("Child status unknown. Restarting.\n");
        } else {
            do_child(parent_pid);
            exit(0);
        }
    }
}

int main() {
    //signal(SIGUSR1, parent_isr);
    __message = (Message*) mmap(NULL, sizeof(Message), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    __message->status = STATUS_ALIVE;
    monitor_loop();
    munmap(__message, sizeof(Message));

    return 0;
}