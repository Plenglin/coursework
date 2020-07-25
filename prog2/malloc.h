/**
 * There are several invariants placed upon the node lists.
 * 
 * - Every manager is located before its employees.
 * - The last employee of one manager is located before the next manager.
 * - No 2 consecutive chunks are free chunks.
 * - The first chunk is a manager chunk.
 * - The last chunk is not a free chunk.
 */

#ifndef __MALLOC_H__
#define __MALLOC_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./data.h"

#define PAGESIZE 4096

typedef unsigned int byte;


Chunk *first_manager, *last_manager;
LeaderTail free_list;


inline void append_last_chunk(Chunk *chunk) {
    Manager *manager = &last_manager->manager;
    ChunkList4 *list = &manager->employees;
    if (list->count == 4) {  // Full!
        // Promote appended to manager
        Manager *new_manager = &chunk->manager;
        chunk->flags = CHUNK_USED | CHUNK_MANAGER;
        new_manager->employees = {0};

        manager->next = chunk;
        new_manager->prev = last_manager;
        new_manager->next = NULL;

        last_manager = chunk; 
        return;
    }

    chunk->flags = CHUNK_USED;
    int i = list->count++;
    list->sizes.a[i] = chunk->size;
    list->locations.a[i] = chunk;
}

inline Chunk *find_best_fit(int size) {
    Chunk *best_fit = NULL;
    int best_size;
    int checked_free_list = 0;
    int chunk_size;

    for (LeaderTail *leader = &free_list; leader != &free_list || !checked_free_list; leader = leader->prev) {
        Chunk *leader_chunk = get_chunk_of_tail(leader);
        checked_free_list = 1;

        // Is the leader chunk open, and will this size fit in this chunk?
        chunk_size = leader_chunk->size;
        if (chunk_size == size) {
            return leader_chunk;
        } else if (size < chunk_size && best_fit == NULL || chunk_size < best_size) {
            best_fit = leader_chunk;
            best_size = chunk_size;
        }

        // Check follower nodes
        ChunkList4 *list = &leader->followers;
        for (int i = 0; i < list->count; i++) {
            // Is the leader chunk open, and will this size fit in this chunk?
            chunk_size = list->sizes.a[i];
            if (chunk_size == size) {
                return list->locations.a[i];
            } else if (size < chunk_size && best_fit == NULL || chunk_size < best_size) {
                best_fit = list->locations.a[i];
                best_size = chunk_size;
            }
        }
    }
}

inline void add_free_chunk(Chunk *chunk, int size) {
    if (push_to_leader(&free_list, chunk, size)) {
        // Free list is full.
        LeaderTail *next = free_list.next;
        if (push_to_leader(next, chunk, size)) {
            // Free list's next is full. This is the new free_list.next
            chunk->flags = CHUNK_LEADER;
            LeaderTail *leader = &chunk->leader;
            next->prev = leader;
            free_list.next = leader;

            leader->prev = &free_list;
            leader->next = next;
            leader->followers = {0};
        }
    }
}

inline void delete_free_chunk(Chunk *chunk) {
    if (chunk->flags & CHUNK_LEADER) {
        pop_leader(&chunk->leader);
    } else {
        pop_follower(&chunk->follower);
    }
}

inline byte is_heap_empty() {
    return first_manager == sbrk(0);
}

byte* mymalloc(int size) {
    // How much space the chunk would take up in memory
    int actual_size = ((HEADSIZE + size + PAGESIZE - 1) / PAGESIZE) * PAGESIZE;

    // Initialize everything if we have an empty heap
    if (is_heap_empty()) {
        free_list.next = &free_list;
        free_list.prev = &free_list;
        free_list.followers = {0};

        first_manager = (Chunk*)sbrk(actual_size);
        first_manager->flags = CHUNK_MANAGER | CHUNK_USED;
        first_manager->size = actual_size;
        last_manager = first_manager;
        
        Manager *manager = &first_manager->manager;
        manager->employees = {0};
        manager->prev = NULL;
        manager->next = NULL;
        return (byte*)first_manager + HEADSIZE;
    }

    // Find the best fit
    Chunk *best_fit = find_best_fit(actual_size);
    
    // No open chunks! Push the program break forward
    if (best_fit == NULL) {
        Chunk *new_chunk = (Chunk*)sbrk(actual_size);
        new_chunk->size = actual_size;
        append_last_chunk(new_chunk);
        return (byte*)new_chunk + HEADSIZE;
    }

    // There are open chunks.
    int current_size = best_fit->size;
    best_fit->flags |= CHUNK_USED;
    best_fit->size = actual_size;
    
    // Remove this chunk from the free chunk list
    delete_free_chunk(best_fit); 

    // Number of unused bytes in the original chunk
    int between_size = current_size - actual_size;

    // Can another chunk fit between this and the next chunk, post-allocation?
    if (between_size > 0) {
        // Create that in-between chunk.
        Chunk *between = (Chunk*) ((char*)best_fit + actual_size);
        between->size = between_size;
        add_free_chunk(between, between_size);
    }
    return (byte*)best_fit + HEADSIZE;
}

/**
 * Given that this_chunk is an employee chunk, finds the bounds of the new free chunk if you were to free 
 * that chunk. If NULL is returned for end_chunk, that means that every chunk outside of start_chunk would 
 * be outside of the program break after freeing.
 */
inline void find_boundary_chunks_of_employee(Chunk *this_chunk, Chunk **start_chunk, Chunk **end_chunk) {
    Chunk *this_chunk_manager_chunk = this_chunk->employee.manager;
    Manager *this_chunk_manager = &this_chunk_manager_chunk->manager;

    // Find the previous chunk.
    int next_chunk_i;
    Chunk *prev_chunk;
    ChunkList4 *list = &this_chunk_manager->employees;
    if (list->locations.a[0] == this_chunk) {  // First element?
        prev_chunk = this_chunk_manager_chunk;  // May be null
        next_chunk_i = 1;
    } else {  // It's somewhere else
        for (int i = 1; i < list->count; i++) {
            if (list->locations.a[i] == this_chunk) {
                prev_chunk = list->locations.a[i - 1];  // Definitely not null
                next_chunk_i = i + 1;
                break;
            }
        }
    }

    if (prev_chunk == NULL || prev_chunk->flags & CHUNK_USED) {
        // This is either the first chunk or the last chunk is used.
        *start_chunk = this_chunk;
    } else {
        // There is a chunk before this.
        *start_chunk = prev_chunk;
    }

    Chunk *next_chunk;
    int next_chunk_size;
    if (next_chunk_i >= list->count) {  
        // Next chunk index is outside of range. This is the last chunk of its manager.
        next_chunk = this_chunk_manager->next;
        next_chunk_size = next_chunk->size;
    } else { 
        // This is not the last chunk, so the next chunk exists.
        next_chunk = list->locations.a[next_chunk_i];
        next_chunk_size = list->sizes.a[next_chunk_i];
    }

    if (next_chunk == NULL) {
        // This is the very last chunk.
        *end_chunk = NULL;
    } else if (next_chunk->flags & CHUNK_USED) {
        // The next chunk is used. Note that end is exclusive.
        *end_chunk = next_chunk;
    } else {
        // The next chunk is free. It cannot be the last chunk due to the invariant.
        *end_chunk = (Chunk*)((char*)next_chunk + next_chunk_size);
    }
}

inline Chunk* find_start_chunk_of_manager(Chunk *this_chunk, Manager *this_chunk_manager) {   
    if (this_chunk_manager->prev == NULL) {  // This is the very first chunk.
        return this_chunk;
    }
    
    // The previous manager might be the physically previous chunk.
    Chunk *prev_chunk = this_chunk_manager->prev;
    Manager *prev_chunk_manager = &prev_chunk->manager;

    // Change phys. prev to manager's last employee if necessary.
    if (prev_chunk_manager->employees.count > 0) {
        int i = prev_chunk_manager->employees.count - 1;
        prev_chunk = prev_chunk_manager->employees.locations.a[i];
    }

    if (prev_chunk->flags & CHUNK_USED) {
        return this_chunk;
    }
    return prev_chunk;
}

inline Chunk* find_end_chunk_of_manager(Chunk *this_chunk, Manager *this_chunk_manager) {   
    if (this_chunk_manager->next == NULL) {  // This is the very last chunk.
        return NULL;  // This chunk is beyond the program break.
    }

    Chunk *next_chunk;
    int next_chunk_size;
    if (this_chunk_manager->employees.count > 0) {
        // There are employees. The first employee is the physically next chunk.
        next_chunk = this_chunk_manager->employees.locations.a[0];
        next_chunk_size = this_chunk_manager->employees.sizes.a[0];
    } else {
        // No employees. The next manager is the physically next chunk.
        next_chunk = this_chunk_manager->next;
        next_chunk_size = next_chunk->size;
    }
    
    // Note that end chunk is exclusive, and that next chunk cannot be physically last chunk due to the invariant.
    if (next_chunk->flags & CHUNK_USED) {
        // Used: do not include next chunk.
        return next_chunk;
    }
    // Free: do include next chunk.
    return (Chunk*)(next_chunk + next_chunk_size);
}

void myfree(byte *addr) {
    Chunk *this_chunk = (Chunk*)(addr - HEADSIZE);

    // Start chunk is inclusive, end chunk is exclusive. 
    Chunk *start_chunk, *end_chunk;
    
    // Probe chunks around this chunk to find start and end.
    Manager *this_chunk_manager;
    if (this_chunk->flags & CHUNK_MANAGER) { 
        // This is a manager chunk
        this_chunk_manager = &this_chunk->manager;

        start_chunk = find_start_chunk_of_manager(this_chunk, this_chunk_manager);
        end_chunk = find_end_chunk_of_manager(this_chunk, this_chunk_manager);
    } else { 
        // This is an employee chunk 
        find_boundary_chunks_of_employee(this_chunk, &start_chunk, &end_chunk);
    }
    
    if (end_chunk == NULL) {
        // This chunk will be beyond the program break!
        if (start_chunk != this_chunk) {
            // We are merging into the previous chunk
            // TODO remove start chunk from list
        }
        brk(start_chunk);
        return;
    }

    // We are merging with next chunk
    // TODO Replace next chunk's free list entry with new superchunk entry.

    if (start_chunk == this_chunk) {  
        // We are not merging into the previous chunk
        // TODO Add this chunk to the free list.
    }

    // TODO We are merging into the previous chunk,
    // Modify the previous chunk's size entry
}

void analyse() {
    if (is_heap_empty()) {
        printf("(empty heap)\n");
        return;
    }
    chunkhead* ch = (chunkhead*)first_chunk;
    for(int no=0; ch; ch = (chunkhead*)ch->next, no++) {
        printf("%d | current addr: %p |", no, ch);
        printf("size: %ld | ", ch->size);
        printf("info: %ld | ", ch->info);
        printf("next: %p | ", ch->next);
        printf("prev: %p", ch->prev);
        printf("      \n");
    }
    printf("program break on address: %x\n",sbrk(0));
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

#endif