#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>


//#include "eisenberg.hpp"

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

char SENTENCE1[] = "Did you ever hear the tragedy of Darth Plagueis The Wise? I thought not. It’s not a story the Jedi would tell you. It’s a Sith legend. Darth Plagueis was a Dark Lord of the Sith, so powerful and so wise he could use the Force to influence the midichlorians to create life… He had such a knowledge of the dark side that he could even keep the ones he cared about from dying. The dark side of the Force is a pathway to many abilities some consider to be unnatural. He became so powerful… the only thing he was afraid of was losing his power, which eventually, of course, he did. Unfortunately, he taught his apprentice everything he knew, then his apprentice killed him in his sleep. Ironic. He could save others from death, but not himself.";
char SENTENCE2[] = "The FitnessGram™ Pacer Test is a multistage aerobic capacity test that progressively gets more difficult as it continues. The 20 meter pacer test will begin in 30 seconds. Line up at the start. The running speed starts slowly, but gets faster each minute after you hear this signal. [beep] A single lap should be completed each time you hear this sound. [ding] Remember to run in a straight line, and run as long as possible. The second time you fail to complete a lap before the sound, your test is over. The test will begin on the word start. On your mark, get ready, start.";
char SENTENCE3[] = "RAID: Shadow Legends™ is an immersive online experience with everything you'd expect from a brand new RPG title. It's got an amazing storyline, awesome 3D graphics, giant boss fights, PVP battles, and hundreds of never before seen champions to collect and customize.";

int main() {
    char src[8192] = {0};
    char *buf = (char*)mmap(NULL, 8192, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buf[0] = 0;
    int index = fork_layers(2, 0);
    switch (index) {
        case 1:
            strcpy(src, SENTENCE1);
            break;
        case 2:
            strcpy(src, SENTENCE3);
            break;
        case 3:
            strcpy(src, SENTENCE3);
            break;
    }
    while (1) {
        if (index == 0) {
            printf("%s\n", buf);
        } else {
            strcpy(buf, src);
        }
    }
}
