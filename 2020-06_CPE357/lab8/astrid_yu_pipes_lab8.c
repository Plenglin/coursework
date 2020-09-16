#include <stdio.h>
#include <sys/mman.h>


typedef struct mypipe {
    char *pipebuffer;
    int buffersize;
    int start_occupied;
    int end_occupied;
} mypipe;

int init_pipe(mypipe *pipe, int size) {
    pipe->pipebuffer = (char*) mmap(
        NULL,
        size,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_SHARED,
        -1,
        0
    );
    pipe->buffersize = size;
    pipe->start_occupied = 0;
    pipe->end_occupied = 0;
}

void free_pipe(mypipe *pipe) {
    munmap(pipe->pipebuffer, pipe->buffersize);
}

int myread(mypipe *pipe, char *buffer, int size) {
    for (int i = 0; i < size; i++) {
        buffer[i] = pipe->pipebuffer[pipe->start_occupied];
        pipe->start_occupied = (pipe->start_occupied + 1) % pipe->buffersize;
    }
}

int mywrite(mypipe *pipe, char *buffer, int size) {
    for (int i = 0; i < size; i++) {
        pipe->pipebuffer[pipe->end_occupied] = buffer[i];
        pipe->end_occupied = (pipe->end_occupied + 1) % pipe->buffersize;
    }
}

int main() {
    char text[100];
    mypipe pipeA;
    init_pipe(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);

    myread(&pipeA, text, 12);
    printf("%s\n", text);
    myread(&pipeA, text, 16);
    printf("%s\n", text);

    mywrite(&pipeA, "and now we test the carryover", 30);
    myread(&pipeA, text, 30);
    printf("%s\n", text);
}