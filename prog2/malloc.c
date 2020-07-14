#include <stdio.h>
#include <stdlib.h>
#include <assert.h>  // for testing purposes only. not used in actual code

#define PAGESIZE 1024
#define HEAPSIZE PAGESIZE * 16 //1048576

typedef struct chunkhead {
    unsigned int size;
    unsigned int info;
    chunkhead *next, *prev;
} chunkhead;

unsigned char myheap[HEAPSIZE];
int program_break_offset = 0;

/**
 * Create the last node at the given location.
 */
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

/**
 * Create a node and automatically link it with prev and next.
 */
void create_chunk(chunkhead *chunk, int pages_count, chunkhead *prev, chunkhead *next) {
    chunk->size = pages_count * PAGESIZE - sizeof(chunkhead);
    chunk->info = 0;

    chunk->prev = prev;
    if (prev != NULL) {
        prev->next = chunk;
    }

    chunk->next = next;
    if (next != NULL && (unsigned char*)next < myheap + HEAPSIZE) {
        next->prev = chunk;
    }
}

/**
 * Sets the memory into a state such that it is completely free.
 */
void initialize() {
    create_program_break(myheap, NULL);
}

unsigned char* mymalloc(int size) {
    for (chunkhead *chunk = (chunkhead*) myheap; chunk != NULL; chunk = chunk->next) {
        // Is the chunk open, and will this size fit in this chunk?
        if (!chunk->info && size <= chunk->size) {
            // # of pages to allocate. ceil((chunkhead size + allocation size) / page size)
            int n_alloc_pages = (sizeof(chunkhead) + size + PAGESIZE - 1) / PAGESIZE;

            chunk->size = n_alloc_pages * PAGESIZE - sizeof(chunkhead);
            chunk->info = 1;

            // Is this before the program break?
            if (chunk->next == NULL) {
                // We are at the program break. Get the new program break location.
                unsigned char* new_brk = ((unsigned char*)chunk + n_alloc_pages * PAGESIZE);
                create_program_break(new_brk, chunk);
            } else {
                unsigned long n_between_pages = ((unsigned long)(chunk->next) - (unsigned long)chunk) / PAGESIZE - n_alloc_pages;

                // Can another chunk fit between this and the next chunk, post-allocation?
                if (n_between_pages > 0) {
                    // Create that in-between chunk.
                    chunkhead *between = (chunkhead*) ((unsigned char*)chunk + n_between_pages * PAGESIZE);
                    create_chunk(between, n_between_pages, chunk, chunk->next);
                }
            }
            return (unsigned char*)chunk + sizeof(chunkhead);
        }
    }
    return NULL;
}

void myfree(unsigned char *addr) {
    chunkhead *chunk = (chunkhead*)(addr - sizeof(chunkhead));
    
    // Bounds of the new free space, including chunkhead. Start inclusive, end exclusive.
    unsigned char *start, *end;

    if (chunk->prev == NULL) {
        start = (unsigned char*)chunk;
    } else {
        start = (unsigned char*)chunk->prev;

        // Exclude the chunk if it's full
        if (chunk->prev->info == 1) {
            start += sizeof(chunkhead) + chunk->prev->size;
        }
    }
    chunkhead *start_chunk = (chunkhead*) start;

    if (chunk->next == NULL) {
        end = addr + chunk->size;
    } else {
        end = (unsigned char*)chunk->next;

        // Include the chunk if it's empty
        if (chunk->next->info == 0) {
            end += sizeof(chunkhead) + chunk->next->size;
        }
    } 

    // Is this going to be the new program break?
    if (end >= myheap + HEAPSIZE) {
        create_program_break(start, start_chunk->prev);
    } else {
        chunkhead *next = (chunkhead*) end;
        int n_pages = (end - start) / PAGESIZE;
        create_chunk(start_chunk, n_pages, start_chunk->prev, next);
    }

}

void analyse() {
    int n = 0;
    for (chunkhead *chunk = (chunkhead*) myheap; chunk != NULL; chunk = chunk->next) {
        printf("chunk %d:\n", n);
        printf("  size: %d\n", chunk->size);
        printf("  info: %d\n", chunk->info);
        printf("  prev: %p\n", chunk->prev);
        printf("  loc: %p\n", chunk);
        printf("  next: %p\n", chunk->next);
        n++;
    }
    putchar('\n');
}

///////////// DEBUGGING FUNCTIONS FOR TESTS ///////////////

int get_n_chunks() {
    int n = 0;
    chunkhead *chunk = (chunkhead*) myheap;
    while (chunk != NULL) {
        chunk = chunk->next;
        n++;
    }
    return n;
}

chunkhead* get_managed_chunk(int i) {
    if (i < 0) i += get_n_chunks();
    chunkhead *chunk = (chunkhead*) myheap;
    while (i > 0) {
        chunk = chunk->next;
        i--;
    }
    return chunk;
}
chunkhead* get_chunk_of(unsigned char *p) {
    return (chunkhead*) (p - sizeof(chunkhead));
}
