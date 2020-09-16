#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>


int main() {
    if (fork() == 0) {
        if (fork() == 0) {
            printf("Astrid ");
            return 0;
        }
        wait(0);
        printf("Augusta ");
        return 0;
    }
    printf("Yu\n");
    wait(0);
    
    return 0;
}
