#include <stdio.h>
#include <stdlib.h>

#define PAGESIZE 1024
#define HEAPSIZE 1048576

typedef struct chunkhead {
    unsigned int size;
    unsigned int info;
    chunkhead *next, *prev;
} chunkhead;

unsigned char myheap[HEAPSIZE];
int program_break_offset = 0;

void create_chunk(chunkhead *chunk, int pages_count, chunkhead *prev, chunkhead *next) {
    chunk->size = pages_count * PAGESIZE - sizeof(chunkhead);
    chunk->info = 0;

    chunk->prev = NULL;
    if (prev != NULL) {
        prev->next = chunk;
    }

    chunk->next = next;
    if (next != NULL) {
        next->prev = chunk;
    }
}

void create_program_break(unsigned char *loc, chunkhead *prev) {
    unsigned long count = myheap + HEAPSIZE - loc;
    unsigned long n_pages_left = count / PAGESIZE;

    // Creating a program break here would exceed heap limits!
    if (n_pages_left <= 0) {
        prev->next = NULL;
        return;
    }

    chunkhead *chunk_brk = (chunkhead*) loc;
    chunk_brk->size = n_pages_left * PAGESIZE - sizeof(chunkhead);
    chunk_brk->prev = prev;
    chunk_brk->next = NULL;
    chunk_brk->info = 0;
    if (prev != NULL) {
        prev->next = chunk_brk;
    }
}

void initialize() {
    create_program_break(myheap, NULL);
}

unsigned char* mymalloc(int size) {
    for (chunkhead *chunk = (chunkhead*) myheap; chunk != NULL; chunk = chunk->next) {
        // Is the chunk open, and will this size fit in this chunk?
        if (!chunk->info && size <= chunk->size) {
            // # of pages to allocate. ceil((chunkhead size + allocation size) / heap size)
            int n_alloc_pages = (sizeof(chunkhead) + size + HEAPSIZE - 1) / HEAPSIZE;

            chunk->size = n_alloc_pages * HEAPSIZE - sizeof(chunkhead);
            chunk->info = 1;

            // Is this before the program break?
            if (chunk->next != NULL) {
                unsigned long n_between_pages = ((unsigned long)(chunk->next) - (unsigned long)chunk - n_alloc_pages) / PAGESIZE;

                // Can another chunk fit between this and the next chunk, post-allocation?
                if (n_between_pages > 0) {
                    // Create that in-between chunk.
                    chunkhead *between = (chunkhead*) ((unsigned char*)chunk + n_between_pages * PAGESIZE);
                    create_chunk(between, n_between_pages, chunk, chunk->next);
                }
            } else {
                // We are at the program break. Get the new program break location.
                unsigned char* new_brk = ((unsigned char*)chunk + n_alloc_pages * PAGESIZE);
                create_program_break(new_brk, chunk);
            }
            return (unsigned char*)chunk + sizeof(chunkhead);
        }
    }
    return NULL;
}

void myfree(unsigned char *addr) {

}

void analyse() {
    int n = 0;
    for (chunkhead *chunk = (chunkhead*) myheap; chunk != NULL; chunk = chunk->next) {
        printf("chunk %d:\n", n);
        printf("- size: %d\n", chunk->size);
        printf("- info: %d\n", chunk->info);
        printf("- prev: %p\n", chunk->prev);
        printf("- next: %p\n", chunk->next);
        n++;
    }
}


int main() {
    initialize();

    unsigned char *a, *b, *c;
    a = mymalloc(1000);
    b = mymalloc(1000);
    c = mymalloc(1000);
    myfree(b);
    myfree(a);
    analyse();

    return 0;
}
