#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct chunkdata
{
    char c;
    int offset;
} chunkdata;

int main() {
    FILE *file = fopen("crypt.bin", "r");
    chunkdata chunk;
    while (1) {
        fread(&chunk.c, 1, sizeof(char), file);
        fread(&chunk.offset, 1, sizeof(int), file);
        if (chunk.c == 0 && chunk.offset == 0) {
            fclose(file);
            return 0;
        }
        printf("%c", chunk.c - chunk.offset);
    }
}