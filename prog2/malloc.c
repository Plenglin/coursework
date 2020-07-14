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

chunkhead *first_chunk = NULL;
chunkhead *last_chunk = NULL;
void *program_break = NULL;
chunkhead free_list = {NULL, NULL, &free_list, &free_list, 0, 0};  // sentinel node

/**
 * Create a node and automatically link it with prev and next.
 */
void set_chunk(chunkhead *chunk, int size, chunkhead *prev, chunkhead *next) {
    chunk->size = size;
    chunk->info = 0;

    chunk->prev = prev;
    if (prev != NULL) {
        prev->next = chunk;
    }

    chunk->next = next;
    if (next != NULL) {
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

byte* mymalloc(int size) {
    // How much space the chunk would take up in memory
    int actual_size = ((sizeof(chunkhead) + size + PAGESIZE - 1) / PAGESIZE) * PAGESIZE;

    // The heap size is 0
    if (first_chunk == program_break) {
        first_chunk = (chunkhead*)sbrk(actual_size);
        last_chunk = first_chunk;
        program_break = sbrk(0);
        set_chunk(first_chunk, actual_size, NULL, NULL);
        return (byte*)first_chunk + sizeof(chunkhead);
    }

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
        chunkhead *new_chunk = (chunkhead*)sbrk(actual_size);
        set_chunk(new_chunk, actual_size, last_chunk, NULL);
        last_chunk = new_chunk;
        program_break = (char*)new_chunk + actual_size;
        return (byte*)new_chunk + sizeof(chunkhead);
    }

    size_t current_size = best_fit->size;
    best_fit->info = 1;
    best_fit->size = actual_size;
    
    // Remove this chunk from the free chunk list
    delete_free_chunk(best_fit); 

    // Number of unused bytes in the original chunk
    size_t between_size = current_size - actual_size;

    // Can another chunk fit between this and the next chunk, post-allocation?
    if (between_size > 0) {
        // Create that in-between chunk.
        chunkhead *between = (chunkhead*) ((char*)best_fit + actual_size);
        set_chunk(between, between_size, best_fit, best_fit->next);
        add_free_chunk(between);
    }
    return (byte*)best_fit + sizeof(chunkhead);
}

void myfree(byte *addr) {
    chunkhead *chunk = (chunkhead*)(addr - sizeof(chunkhead));

    // Is this the very last chunk?
    if (chunk->next == NULL) {
        // delet this
        last_chunk = chunk->prev;
        if (chunk->prev != NULL) {
            chunk->prev->next = NULL;
        }
        sbrk(-chunk->size);
        return;
    }
    
    // Determine the bounds of the new free space.

    // End exclusive
    char *end = (char*)chunk->next;

    // Include the next chunk if it's empty
    if (chunk->next->info == 0) {
        end += chunk->next->size;
    }

    // Start inclusive
    chunkhead *start_chunk;
    byte should_add_to_list = 0;
    if (chunk->prev == NULL) {
        start_chunk = chunk;
    } else {
        start_chunk = chunk->prev;

        // Exclude the previous chunk if it's full
        if (chunk->prev->info == 1) {
            start_chunk = (chunkhead*)((char*)start_chunk + chunk->prev->size);
            add_free_chunk(start_chunk);
        }
    }

    chunkhead *next = (chunkhead*) end;
    set_chunk(start_chunk, end - (char*)start_chunk, start_chunk->prev, next);
}

void analyse() {
    int n = 0;
    for (chunkhead *chunk = first_chunk; chunk != NULL; chunk = chunk->next) {
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

void analyse_free() {
    int n = 0;
    for (chunkhead *chunk = free_list.next_free; chunk != &free_list; chunk = chunk->next_free) {
        printf("chunk %d:\n", n);
        printf("  size: %d\n", chunk->size);
        printf("  info: %d\n", chunk->info);
        printf("  loc: %p\n", chunk);
        printf("  prev: %p\n", chunk->prev);
        printf("  next: %p\n", chunk->next);
        printf("  prev_free: %p\n", chunk->prev_free);
        printf("  next_free: %p\n", chunk->next_free);
        n++;
    }
    putchar('\n');
}

///////////// DEBUGGING FUNCTIONS FOR TESTS ///////////////

int get_n_chunks() {
    int n = 0;
    chunkhead *chunk = first_chunk;
    while (chunk != NULL) {
        chunk = chunk->next;
        n++;
    }
    return n;
}

chunkhead* get_managed_chunk(int i) {
    if (i < 0) i += get_n_chunks();
    chunkhead *chunk = first_chunk;
    while (i > 0) {
        chunk = chunk->next;
        i--;
    }
    return chunk;
}
chunkhead* get_chunk_of(unsigned char *p) {
    return (chunkhead*) (p - sizeof(chunkhead));
}
