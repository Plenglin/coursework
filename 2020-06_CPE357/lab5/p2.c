#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0) { 
        if (fork() == 0) {
            printf("5");
            return 0;
        }
        wait(0);
        printf("4");
        return 0;
    }
    
    wait(0);
    if (fork() == 0) {
        printf("3");
        return 0;
    }
    wait(0);
    if (fork() == 0) {
        printf("2");
        return 0;
    }
    wait(0);
    printf("1");

    return 0;
}