#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGESIZE 4096

typedef unsigned int byte;

typedef struct chunkhead {
    /**
     * The chunk placed physically after this, or NULL if this is the last chunk.
     */
    chunkhead *next;
    
    /**
     * The chunk placed physically before this, or NULL if this is the first chunk.
     */
    chunkhead *prev;

    /**
     * The next free node in the free node linked list. Never NULL.
     */
    chunkhead *next_free;

    /**
     * The previous free node in the free node linked list. Never NULL.
     */
    chunkhead *prev_free;

    /**
     * The size of this chunk, INCLUDING the chunkhead.
     */
    size_t size;

    /**
     * 1 if occupied, 0 if free.
     */
    byte info;
} chunkhead;

chunkhead *first_chunk = NULL;
chunkhead *last_chunk = NULL;

/**
 * A doubly-linked list containing all the free chunks currently in the memory management system.
 * If there were more random accesses and random-sized malloc()'s then I would use a BST keyed by
 * the node's size instead. However, this list would likely be more optimal for the performance  
 * test that is provided, because it has a large number of evenly-sized malloc's and sequential 
 * free's, and the memory doesn't really get fragmented too much.
 */
chunkhead free_list = {NULL, NULL, &free_list, &free_list, 0, 0}; 

/**
 * Create a node and automatically link it with prev and next.
 */
inline void set_chunk(chunkhead *chunk, int size, chunkhead *prev, chunkhead *next, byte info) {
    chunk->size = size;
    chunk->info = info;

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
inline void add_free_chunk(chunkhead *chunk) {
    chunkhead *next = free_list.next_free;    
    chunkhead *prev = &free_list;

    chunk->prev_free = prev;
    chunk->next_free = next;
    prev->next_free = chunk;
    next->prev_free = chunk;    
}

/**
 * Removes a free chunk from the list of free chunks.
 */
inline void delete_free_chunk(chunkhead *chunk) {
    chunkhead *prev = chunk->prev_free;
    chunkhead *next = chunk->next_free;
    prev->next_free = next;
    next->prev_free = prev;
}

inline byte is_heap_empty() {
    return first_chunk == sbrk(0) || first_chunk == NULL;
}

byte* mymalloc(int size) {
    // How much space the chunk would take up in memory
    int actual_size = ((sizeof(chunkhead) + size + PAGESIZE - 1) / PAGESIZE) * PAGESIZE;

    if (is_heap_empty()) {
        first_chunk = (chunkhead*)sbrk(actual_size);
        last_chunk = first_chunk;
        free_list.next_free = &free_list;
        free_list.prev_free = &free_list;
        set_chunk(first_chunk, actual_size, NULL, NULL, 1);
        return (byte*)first_chunk + sizeof(chunkhead);
    }

    chunkhead *best_fit = NULL;
    size_t best_size;

    for (chunkhead *chunk = free_list.prev_free; chunk != &free_list; chunk = chunk->prev_free) {
        // Is the chunk open, and will this size fit in this chunk?
        int chunk_size = chunk->size;
        if (chunk_size == actual_size) {
            best_fit = chunk;
            break;
        } else if (actual_size < chunk_size && best_fit == NULL || chunk_size < best_size) {
            best_fit = chunk;
            best_size = chunk_size;
        }
    }
    
    // No open chunks! Push the program break forward
    if (best_fit == NULL) {
        chunkhead *new_chunk = (chunkhead*)sbrk(actual_size);
        set_chunk(new_chunk, actual_size, last_chunk, NULL, 1);
        last_chunk = new_chunk;
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
        set_chunk(between, between_size, best_fit, best_fit->next, 0);
        add_free_chunk(between);
    }
    return (byte*)best_fit + sizeof(chunkhead);
}

void myfree(byte *addr) {
    chunkhead *chunk = (chunkhead*)(addr - sizeof(chunkhead));

    // Start inclusive
    chunkhead *start_chunk;
    byte should_add_to_list = 0;
    chunkhead *prev = chunk->prev;
    if (prev == NULL) {  // We're freeing the very first chunk
        start_chunk = chunk;
        should_add_to_list = 1;
    } else if (prev->info == 1) {  // The prev one is occupied
        start_chunk = chunk;
        should_add_to_list = 1;
    } else {   // The prev one is NOT occupied
        start_chunk = prev;
    }

    chunkhead *end = chunk->next;
    byte should_change_brk = 0;
    if (end == NULL) {  // Are we freeing the last chunk?
        should_change_brk = 1;
    } else {
        // Include the next chunk if next is empty
        if (end->info == 0) {
            end = (chunkhead*) ((char*)end + end->size);
        }

        // Will we be freeing up to the break?
        if (end == sbrk(0)) {
            should_change_brk = 1;
        }
    }

    prev = start_chunk->prev;
    if (should_change_brk) {
        brk(start_chunk);

        // Designate the chunk before this as the last chunk
        if (prev != NULL) {    
            prev->next = NULL;
            last_chunk = prev;
        } 
        return;
    }

    size_t free_space_size = (char*)end - (char*)start_chunk;
    set_chunk(start_chunk, free_space_size, prev, end, 0);
    if (should_add_to_list) {
        add_free_chunk(start_chunk);
    }
}

void analyse() {
    if (is_heap_empty()) {
        //printf("(empty heap)\n");
        return;
    }
    int n = 0;
    for (chunkhead *chunk = first_chunk; chunk != NULL; chunk = chunk->next) {
        printf("chunk %d:\n", n);
        printf("  size: %ld\n", chunk->size);
        printf("  info: %d\n", chunk->info);
        printf("  prev: %p\n", chunk->prev);
        printf("  loc: %p\n", chunk);
        printf("  next: %p\n", chunk->next);
        n++;
    }
    putchar('\n');
}

/**
 * Prints out only the elements of the free list.
 */
void analyse_free() {
    int n = 0;
    for (chunkhead *chunk = free_list.next_free; chunk != &free_list; chunk = chunk->next_free) {
        printf("chunk %d:\n", n);
        printf("  size: %ld\n", chunk->size);
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
