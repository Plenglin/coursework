#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGESIZE 4096

typedef unsigned int byte;

typedef struct chunkhead {
    chunkhead *next, *prev, *next_free, *prev_free;
    /**
     * The size of this chunk, INCLUDING the chunkhead.
     */
    size_t size;
    byte info;
} chunkhead;

void *heap_start = NULL;
void *program_break = NULL;
chunkhead free_list = {NULL, NULL, NULL, NULL, 0, 0};  // sentinel node
chunkhead *last_chunk = NULL;

/**
 * Create the last node at the given location.
 */
void create_program_break(byte *loc, chunkhead *prev) {
    unsigned long count = heap_start + HEAPSIZE - loc;
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
void create_chunk(chunkhead *chunk, int size, chunkhead *prev, chunkhead *next) {
    chunk->size = size;
    chunk->info = 0;

    chunk->prev = prev;
    if (prev != NULL) {
        prev->next = chunk;
    }

    chunk->next = next;
    if (next != NULL && (unsigned char*)next < program_break) {
        next->prev = chunk;
    }
}

/**
 * Adds a free chunk to the list of free chunks.
 */
void add_free_chunk(chunkhead *chunk) {
    chunkhead *next = free_list.next;
    chunkhead *prev = &free_list;

    chunk->prev_free = prev;
    chunk->next_free = next;
    prev->next = chunk;
    next->prev = chunk;    
}

/**
 * Removes a free chunk from the list of free chunks.
 */
void delete_free_chunk(chunkhead *chunk) {
    chunk->prev_free->next_free = chunk->next_free;
    chunk->next_free->prev_free = chunk->prev_free;
}

/**
 * Sets the memory into a state such that it is completely free.
 */
void initialize() {
    if (heap_start == NULL) {
        heap_start = sbrk(0);
    }
    program_break = heap_start;
    brk(program_break);
    free_list.next_free = NULL;
    free_list.prev_free = NULL;
}

void* mymalloc(int size) {
    // How much space the chunk would take up in memory
    int actual_size = ((sizeof(chunkhead) + size + PAGESIZE - 1) / PAGESIZE) * PAGESIZE;

    chunkhead *best_fit = NULL;
    for (chunkhead *chunk = free_list.next_free; chunk != &free_list; chunk = chunk->next_free) {
        // Is the chunk open, and will this size fit in this chunk?
        if (!chunk->info) {
            int chunk_size = chunk->size;
            if (chunk_size == actual_size) {
                best_fit = chunk;
                break;
            } else if (actual_size < chunk_size && chunk_size < best_fit->size) {
                best_fit = chunk;
            }
        }
    }
    
    // No open chunks! Push the program break forward
    if (best_fit == NULL) {
        program_break = sbrk(actual_size);
        create_chunk((chunkhead*) program_break, actual_size, last_chunk, NULL);
        return (char*)program_break + sizeof(chunkhead);
    }

    size_t current_size = best_fit->size;
    best_fit->info = 1;
    
    // Remove this chunk from the free chunk list
    delete_free_chunk(best_fit); 

    // Number of unused bytes in the original chunk
    size_t between_size = current_size - actual_size;

    // Can another chunk fit between this and the next chunk, post-allocation?
    if (between_size > 0) {
        // Create that in-between chunk.
        chunkhead *between = (chunkhead*) ((char*)best_fit + actual_size);
        create_chunk(between, between_size, best_fit, best_fit->next);
    }
    return (void*)((char*)best_fit + sizeof(chunkhead));
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
    if (end >= heap_start + HEAPSIZE) {
        create_program_break(start, start_chunk->prev);
    } else {
        chunkhead *next = (chunkhead*) end;
        int n_pages = (end - start) / PAGESIZE;
        create_chunk(start_chunk, n_pages, start_chunk->prev, next);
    }

}

void analyse() {
    int n = 0;
    for (chunkhead *chunk = (chunkhead*) heap_start; chunk != NULL; chunk = chunk->next) {
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
    chunkhead *chunk = (chunkhead*) heap_start;
    while (chunk != NULL) {
        chunk = chunk->next;
        n++;
    }
    return n;
}

chunkhead* get_managed_chunk(int i) {
    if (i < 0) i += get_n_chunks();
    chunkhead *chunk = (chunkhead*) heap_start;
    while (i > 0) {
        chunk = chunk->next;
        i--;
    }
    return chunk;
}
chunkhead* get_chunk_of(unsigned char *p) {
    return (chunkhead*) (p - sizeof(chunkhead));
}
