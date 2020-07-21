#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


typedef struct datafile
{
char type; //
int size;//size of the data part in bytes
} datafile;

int main() {
    FILE *input = fopen("file.bin", "rb");
    if (input == NULL) {
        return 1;
    }
    
    datafile header;
    char* data;

    fread(&header.type, 1, sizeof(char), input);
    fread(&header.size, 1, sizeof(int), input); 
    data = (char*) malloc(header.size);
    fread(data, 1, header.size, input);
    fclose(input);

    int step;
    switch (header.type) {
        case 'U':
            step = 4;
            break;
        case 'T':
            step = -2;
            break;
        case 'M':
            step = 9;
            break;
        default:
            return 1;
    }

    for (int i = 0; i < header.size; i++) {
        if (data[i] == 10 || data[i] == 0) continue;
        data[i] = data[i] + step;
    }

    FILE *output = fopen("out.txt", "wb");
    for (int i = 0; i < header.size; i++) {
        fwrite(data, 1, header.size, output);
    }
    fclose(output);

    free(data);
}