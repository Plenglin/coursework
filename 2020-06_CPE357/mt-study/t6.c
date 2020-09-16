#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>


char *words[10] = {0};
int sizes[10];

void printWords() {
    printf("words:");
    for (int i = 0; i < 10; i++) {
        if (words[i]) {
            printf(" %s", words[i]);
        }
    }
    printf("\n");
}

int getWordIndex(char *word) {
    for (int i = 0; i < 10; i++) {
        if (words[i] != NULL && !strcmp(words[i], word)) {
            return i;
        }
    }
    return -1;
}

/**
 * Undefined behavior when number of stored words exceeds 10
 */
void insertWord(char *word, int size) {
    for (int i = 0; i < 10; i++) {
        if (words[i] == NULL) {
            words[i] = word;
            sizes[i] = size;
            return;
        }
    }
}

void handleWord(char *word) {
    if (!strcmp(word, "print")) {
        printWords();
        return;
    }
    int index = getWordIndex(word);
    if (index >= 0) {  // word exists
        munmap(words[index], sizes[index] + 1);
        words[index] = NULL;
    } else {
        int size = strlen(word);
        char* heapWord = (char*) mmap(
            NULL,
            size + 1,
            PROT_READ | PROT_WRITE,
            MAP_ANONYMOUS | MAP_SHARED,
            -1,
            0
        );
        strcpy(heapWord, word);
        insertWord(heapWord, size);
    }
}

int main() {
    char buf[1000];
    while (1) {
        scanf("%s", buf);
        handleWord(buf);
    }
}