#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>

#include "eisenberg.hpp"

#define FORK_LAYERS 2

/**
 * Forks however many times you specify. Outputs the index of your process.
 * 0 is the parent. You will have 2^layers processes in the end.
 */
int fork_layers(int layers, int i) {
    int index = (i << 1) | !fork();
    if (layers <= 1) {
        return index;
    }
    return fork_layers(layers - 1, index);
}

char SENTENCE1[] = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. ";
char SENTENCE2[] = "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues.";
char SENTENCE3[] = "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. ";

int main() {
    auto mutex = Eisenberg(1 << FORK_LAYERS);
    
    char src[8192] = {0};
    char *buf = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buf[0] = 0;

    int index = fork_layers(FORK_LAYERS, 0);
    mutex.set_i(index);
    switch (index) {
        case 1:
            sprintf(src, "%d %s", index, SENTENCE1);
            break;
        case 2:
            sprintf(src, "%d %s", index, SENTENCE2);
            break;
        case 3:
            sprintf(src, "%d %s", index, SENTENCE3);
            break;
    }

    for (int i = 0;; i++) {
        auto _lock = mutex.lock();
        if (index % 2 == 0) {
            printf("%d: %s\n", i, buf);
        } else {
            strcpy(buf, src);
        }
    }

    if (index != 0) {
        exit(0);
    }
    wait(0);
    munmap(buf, 8192);
}
