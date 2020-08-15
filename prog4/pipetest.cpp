#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>


int tmp;
int fd[2];

void handle(int sig) {
    close(fd[1]);
    tmp = dup(STDIN_FILENO);
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
}

int main() {
    int *parent_pid = (int*)mmap(
        nullptr,
        sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    *parent_pid = getpid();
    pipe(fd);

    char text[1000];

    if (fork()) {  // parent
        signal(SIGUSR1, handle);
        scanf("%s", text);
        printf("%s\n", text);
        dup2(tmp, STDIN_FILENO);
        scanf("%s", text);
        printf("%s\n", text);
    } else {
        sleep(5);
        kill(*parent_pid, SIGUSR1);
        close(fd[0]);
        write(fd[1], "test", 5);
        close(fd[1]);
    }
}