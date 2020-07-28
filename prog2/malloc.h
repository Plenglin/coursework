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


Chunk *first_manager = NULL;
Chunk *last_manager;
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
                list->occupied.a[i] = 1;  // Set it to occupied
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
    return first_manager == sbrk(0) || first_manager == NULL;
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
        insert_new_chunk(best_fit, between_size, between, 0);
        add_free_chunk(between, between_size);
    }
    return (byte*)best_fit + HEADSIZE;
}

#define MERGE_FORWARD 0x1
#define MERGE_BACKWARD 0x2

#define MERGE_CHUNK_RM_THIS 0x4
#define MERGE_CHUNK_RM_NEXT 0x8
#define MERGE_FREE_RM_NEXT 0x10
#define MERGE_FREE_ADD_THIS 0x20

#define NEIGHBOR_TYPE_NULL 0x0
#define NEIGHBOR_TYPE_MANAGER 0x1
#define NEIGHBOR_TYPE_EMPLOYEE 0x2

struct NeighborChunksResult {
    Chunk *prev;
    Chunk *next;
    int prev_size;
    int next_size;
    char prev_type;
    char next_type;
};

/**
 * Given that this_chunk is an employee chunk, finds the bounds of the new free chunk if you were to free 
 * that chunk. If NULL is returned for end_chunk, that means that every chunk outside of start_chunk would 
 * be outside of the program break after freeing.
 */
inline void find_neighbor_chunks_for_employee(
    Chunk *this_chunk, 
    NeighborChunksResult *result
) {
    Chunk *this_chunk_manager_chunk = this_chunk->employee.manager;
    Manager *this_chunk_manager = &this_chunk_manager_chunk->manager;

    // Find the previous chunk and potential next chunk location.
    int next_chunk_i;
    ChunkList4 *list = &this_chunk_manager->employees;
    if (list->locations.a[0] == this_chunk) {  
        // This is first employee. Prev is the manager.
        result->prev = this_chunk_manager_chunk;  // May be null
        result->prev_type = NEIGHBOR_TYPE_MANAGER;
        next_chunk_i = 1;
    } else {  
        // This is non-first employee.
        result->prev_type = NEIGHBOR_TYPE_EMPLOYEE;
        for (int i = 1; i < list->count; i++) {
            if (list->locations.a[i] == this_chunk) {
                result->prev = list->locations.a[i - 1];  // Definitely not null
                next_chunk_i = i + 1;
                break;
            }
        }
    }

    // Find the next chunk.
    if (next_chunk_i >= list->count) {  
        // Next chunk index is outside of range. This is the last employee of its manager.
        result->next = this_chunk_manager->next;
        result->next_size = result->next->size;
        result->next_type = result->next ? NEIGHBOR_TYPE_MANAGER : NEIGHBOR_TYPE_NULL;
        // Status is clear here, so it will be NULL.
    } else { 
        // This is not the last chunk, so the next chunk exists.
        result->next = list->locations.a[next_chunk_i];
        result->next_size = list->sizes.a[next_chunk_i];
        result->next_type = NEIGHBOR_TYPE_EMPLOYEE;
    }
}

inline void find_prev_for_chunk_manager(Chunk *this_chunk, Manager *this_chunk_manager, NeighborChunksResult *result) {   
    if (this_chunk_manager->prev == NULL) {  
        // This is the very first chunk.
        result->prev = NULL;
        result->prev_type = NEIGHBOR_TYPE_NULL;
        return;
    }
    
    // The previous manager might be the physically previous chunk.
    Chunk *prev_chunk = this_chunk_manager->prev;
    Manager *prev_chunk_manager = &prev_chunk->manager;

    // Change phys. prev to manager's last employee if necessary.
    ChunkList4 *list = &prev_chunk_manager->employees;
    if (list->count > 0) {
        // Prev is last employee
        int i = prev_chunk_manager->employees.count - 1;
        result->prev = list->locations.a[i];
        result->prev_size = list->sizes.a[i];  // No need to load its page for this!
        result->prev_type = NEIGHBOR_TYPE_EMPLOYEE;
    } else {
        // Prev is the manager
        result->prev = prev_chunk;
        result->prev_size = prev_chunk->size;
        result->prev_type = NEIGHBOR_TYPE_MANAGER;
    }
}

inline void find_next_for_chunk_manager(Chunk *this_chunk, Manager *this_chunk_manager, NeighborChunksResult *result) {   
    // Check if next is first employee
    ChunkList4 *list = &this_chunk_manager->employees;
    if (list->count > 0) {
        // List has elements, so next is first employee
        result->next = list->locations.a[0];
        result->next_size = list->sizes.a[0];  // No need to load its page!
        result->next_type = NEIGHBOR_TYPE_EMPLOYEE;
        return;
    }
    // No employees. Next is next manager
    result->next = this_chunk_manager->next;
    result->next_size = result->next->size;
    result->next_type = result->next ? NEIGHBOR_TYPE_MANAGER : NEIGHBOR_TYPE_NULL;
}

/**
 * If this chunk is an employee and the next one is an employee, possibly merges previous, this, and current.
 */
inline void do_merge_single_manager(Chunk *this_chunk, NeighborChunksResult *result) {
    Chunk *manager_chunk = this_chunk->employee.manager;
    //remove_loc_from(manager_chunk->manager, find)
}

void myfree(byte *addr) {
    Chunk *this_chunk = (Chunk*)(addr - HEADSIZE);

    Chunk *start_chunk, *next_chunk;
    void *end_bound;  // end of new chunk, exclusive
    
    // Probe chunks around this chunk to find start and end.
    Manager *this_chunk_manager;
    NeighborChunksResult neighbors;
    if (this_chunk->flags & CHUNK_MANAGER) { 
        // This is a manager chunk
        this_chunk_manager = &this_chunk->manager;

        if (!this_chunk_manager->next && !this_chunk_manager->prev && !this_chunk_manager->employees.count) {
            // This is the only manager and it has no employees. Clear the heap.
            brk(first_manager);
            return;
        }

        find_next_for_chunk_manager(this_chunk, this_chunk_manager, &neighbors);
        find_prev_for_chunk_manager(this_chunk, this_chunk_manager, &neighbors);
    } else { 
        // This is an employee chunk 
        find_neighbor_chunks_for_employee(this_chunk, &neighbors);
    }
    
    if (end_bound == NULL) {
        // This chunk will be beyond the program break!
        if (merge_status & MERGE_BACKWARD) {
            // We are merging into the previous chunk. Remove start chunk from free list.
            pop_chunk_from_free_list(start_chunk);
        }

        if (this_chunk->flags & CHUNK_MANAGER) { 
            //  
        }
        brk(start_chunk);
        return;
    }

    int merge_status = 0;
   
    if (!(neighbors.next->flags & CHUNK_USED)) {
        merge_status |= MERGE_FORWARD;
    }
    if (!(neighbors.prev->flags & CHUNK_USED)) {
        merge_status |= MERGE_BACKWARD;
    }


    if (merge_status & MERGE_FORWARD) {
        // We are merging with next chunk
        // TODO Replace next chunk's free list entry with new superchunk entry.
    }

    if (merge_status & MERGE_BACKWARD) {
        // TODO We are merging into the previous chunk
        // Modify the previous chunk's size entry
    } else {
        // We are not merging into the previous chunk
        // TODO Add this chunk to the free list.
    }
}

void print_list4(ChunkList4 *list) {
    printf("n:%d", list->count);
    for (int i = 0; i < list->count; i++) {
        printf(" (%d:%p:%d)", i, list->locations.a[i], list->sizes.a[i]);
    }
}

void print_arr4(ChunkList4 *list) {
    for (int i = 0; i < 4; i++) {
        printf(" (%d:%p:%d)", i, list->locations.a[i], list->sizes.a[i]);
    }
}

void print_chunk(Chunk *chunk, int i, int indent) {
    if (indent) {
        printf("  ");
    }
    printf("%d | current addr: %p | size: %d | ", i, chunk, chunk->size);

    if (chunk->flags & CHUNK_USED) {
        printf("used ");
    } else {
        printf("free ");
    }

    if (chunk->flags & CHUNK_MANAGER) {
        Manager *manager = &chunk->manager;
        printf("manager(p:%p n:%p ", manager->prev, manager->next);
        print_list4(&manager->employees);
        printf(") ");
    } else {
        printf("employee(m:%p) ", chunk->employee.manager);
    }

    if (!(chunk->flags & CHUNK_USED)) {
        if (chunk->flags & CHUNK_LEADER) {
            LeaderTail *leader = &chunk->leader;
            printf("leader(p:%p n:%p ", leader->prev, leader->next);
            print_arr4(&leader->followers);
            printf(")");
        } else {
            FollowerTail *follower = &chunk->follower;
            printf("follower(l:%p, i:%d)", follower->leader, follower->index);
        }
    }

    printf("\n");
}

void analyse() {
    if (is_heap_empty()) {
        printf("(empty heap)\n");
        return;
    }
    
    int i = 0;
    for (Chunk *manager_chunk = first_manager; manager_chunk; manager_chunk = manager_chunk->manager.next) {
        Manager *manager = &manager_chunk->manager;
        print_chunk(manager_chunk, i, 0);
        i++;
        for (int j = 0; j < 4; j++) {
            print_chunk(manager->employees.locations.a[i], i, 1);
            i++;
        }
    }
    printf("program break on address: %p\n",sbrk(0));
}

#endif