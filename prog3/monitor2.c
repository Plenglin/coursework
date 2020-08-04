#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "child.h"

#define SLEEP_SECONDS 10  // TODO change this

void parent_isr(int sig) {
    // noop
}


void monitor_loop() {
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
        printf("Child process terminated.\n");
        switch (__message->status) {
            case STATUS_QUIT:
                printf("Child process quit. Exiting.\n");
                return;
            default:
                printf("Child process status unknown. Forking again in 10 seconds.\n");
                sleep(SLEEP_SECONDS);
                break;
        }
    }
}

int main() {
    signal(SIGUSR1, parent_isr);
    __message = (Message*) mmap(NULL, sizeof(Message), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    __message->status = STATUS_ALIVE;
    monitor_loop();
    munmap(__message, sizeof(Message));
    return 0;
}