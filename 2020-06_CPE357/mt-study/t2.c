#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    char *output = (char*) mmap(
        NULL,
        27, 
        PROT_READ | PROT_WRITE, 
        MAP_ANONYMOUS | MAP_SHARED, 
        -1,
        0
    );

    if (fork() == 0) {
        if (fork() == 0) {
            strncpy(output, "Astrid ", 7);
            exit(0);
        } else {
            strncpy(output + 7, "A. ", 3);
            exit(0);
        }
    } else {
        if (fork() == 0) {
            strncpy(output + 10, "Yu ", 3);
            exit(0);
        } else {
            strncpy(output + 13, "SanFrancisco\n", 14);  // includes the terminator
        }
    }
    wait(0);

    printf(output);

    munmap(output, 27);
}