/**
 * This header file contains data-holding structs used for malloc, as well as utilities
 * for manipulating the structs.
 * 
 * My implementation of malloc uses 2 sets of lists: a list that keeps a record of all the 
 * chunks in order, and a list that only keeps track of the free nodes. Both of these lists
 * are implemented using an unrolled linked list data structure to improve cache efficiency.
 * 
 * Standard linked list time: 160us
 * Unrolled linked list time: ???
 */

#ifndef __DATA_H__
#define __DATA_H__
#include <immintrin.h>

#define PAGESIZE 4096
#define HEADSIZE 128

#define CHUNK_USED 1
#define CHUNK_MANAGER 2
#define CHUNK_LEADER 4

#define SENTINEL_LEADER_LIST_CHUNKS 16
#define HEAP_LEADER_LIST_CHUNKS 4

#define PERM_MM_128_INSERT_0 _mm_set_epi32(2,1,0,3)
#define PERM_MM_128_INSERT_1 _mm_set_epi32(2,1,3,0)
#define PERM_MM_128_INSERT_2 _mm_set_epi32(2,3,1,0)
#define PERM_MM_128_INSERT_3 _mm_set_epi32(3,2,1,0)

#define PERM_MM_256_INSERT_0 _MM_PERM_CBAD
#define PERM_MM_256_INSERT_1 _MM_PERM_CBDA
#define PERM_MM_256_INSERT_2 _MM_PERM_CDBA
#define PERM_MM_256_INSERT_3 _MM_PERM_DCBA

struct Chunk;

/**
 * 4 int array, with 4x128 bit register backing
 */
typedef union Arr4i {
    int a[4];
    __m128i _reg;
} Arr4i;

/**
 * 4 pointer array, with 1x256 bit register backing
 */
typedef union Arr4p {
    Chunk *a[4];
    __m256i _reg;
} Arr4p;

/**
 * 4 byte array, with 1x32 bit register backing
 */
typedef union Arr4b {
    char a[4];
    int _reg;
} Arr4b;

/**
 * A array list of 4 chunks, with 2x256 bit register backing
 */
typedef union ChunkList4 {
    struct {
        unsigned short count;
        Arr4b occupied;
        Arr4i sizes;
        Arr4p locations;
    };
    __m256i _reg[2];
} ChunkList4;

/**
 * A chunk that contains a list of used chunks.
 */
typedef struct Manager {
    Chunk *prev;
    Chunk *next;
    ChunkList4 employees;
} Manager;

/**
 * A chunk that is recorded in a Manager.
 */
typedef struct Employee {
    Chunk *manager;
    // Note that we do not keep track of the index. This is because the index will around, and 
    // we don't want to perform a ton of reindexing, which is cache-inefficient.
} Employee;

/**
 * The tail of a free chunk that contains a list of other free chunks.
 */
typedef struct LeaderTail {
    union {
        struct {
            LeaderTail *prev;
            LeaderTail *next;
        };
        __m128i _regps;
    };
    ChunkList4 followers;
} LeaderTail;

/**
 * The tail of a free chunk that is listed in a Leader chunk.
 */
typedef struct FollowerTail {
    LeaderTail *leader;
    /**
     * Note that we ARE recording the index here. This is because it does not move around in its leader.
     */
    int index;
} FollowerTail;

/**
 * A chunk that is either free or used, and may manage a free list or a used list.
 */
typedef struct Chunk {
    int flags;
    unsigned int size;
    union {
        Manager manager;
        Employee employee;
    };
    // Everything below this line ONLY EXISTS ON FREE CHUNKS. Everything above perfectly fits in 128 bytes.
    union {
        LeaderTail leader;
        FollowerTail follower;
    };
} Chunk;

inline Chunk* get_chunk_of_tail(LeaderTail *leader) {
    return (Chunk*) ((char*)leader - 128);
}

inline void copy_chunklist4(ChunkList4 *dst, ChunkList4 *src) {
    long *ad = (long*) dst;
    long *as = (long*) src;
    ad[0] = as[0];
    ad[1] = as[1];
    ad[2] = as[2];
    ad[3] = as[3];
}

/**
 * Insert into a single array list, deleting the last item if space exceeded.
 */
inline void insert_ordered_arr4(ChunkList4 *list, int size, Chunk *location, char occupied) {
    int i = list->count;
    int i_next;

    // Traverse list in descending order
    for (; i > 0; i = i_next) {
        i_next = i - 1;
        Chunk *loc_next = list->locations.a[i_next];
        if (loc_next < location) {   // Correct spot!
            break;
        }
        list->locations.a[i] = loc_next;
        list->sizes.a[i] = list->sizes.a[i_next];
    }
    // If no correct spot is found, i will be 0 here.
    list->locations.a[i] = location;
    list->sizes.a[i] = size;

    int allow_mask = -1 << (8 * i);
    list->occupied._reg = list->occupied._reg | ((list->occupied._reg & ~allow_mask) >> 8);
    list->occupied.a[i] = occupied;
}

inline void set_manager_chunk(Chunk *chunk, Chunk *prev, Chunk *next) {
    next->manager.prev = prev;
    next->manager.next = next;
    next->flags |= CHUNK_MANAGER;
    next->manager.employees.count = 0;
}

inline void set_employee_chunk(Chunk *chunk, Chunk *manager, int size) {
    chunk->flags &= ~CHUNK_MANAGER;  // Clear manager flag to designate that it's an employee
    chunk->employee.manager = manager;
    chunk->size = size;
}

inline void maybe_set_manager_prev(Chunk *chunk, Chunk *prev) {
    if (chunk) {
        chunk->manager.prev = prev;
    }
}

/**
 * Given that this chunk is a manager, inserts the provided chunk in the correct location.
 */
void insert_new_chunk(Chunk* this_chunk, int size, Chunk *location, char occupied) {
    Arr4i sizes;
    Arr4p locations;
    Manager *manager = &this_chunk->manager;
    ChunkList4 *employees = &manager->employees;

    if (employees->count == 4) {
        // Max size, promote last node to manager
        int size = sizes.a[3];
        Chunk *next = locations.a[3];  // The new next
        maybe_set_manager_prev(manager->next, next);
        set_manager_chunk(next, this_chunk, manager->next);
        manager->next = next;
    } else {
        employees->count++;
    }
    insert_ordered_arr4(employees, size, location, occupied);
    set_employee_chunk(location, this_chunk, size);
}

void remove_loc_from(ChunkList4 *list, Chunk* location, int n) {
    int i;
    for (int i = 0; list->locations.a[i] != location; i++) {  // Move i to the location
        // noop
    }
    int found = i;
    for (; i < list->count; i++) {
        int src = i + n;
        list->locations.a[i] = list->locations.a[src];
        list->sizes.a[i] = list->sizes.a[src];
    }
    int write_mask = -1 << (8 * found);
    list->occupied._reg = 
        list->occupied._reg & write_mask | 
        (list->occupied._reg << (8 * n)) & ~write_mask;
    list->count -= n;
}

/**
 * Push an element onto the leader node.
 */
int push_to_leader(LeaderTail *leader, Chunk *location, int size) {
    ChunkList4 *arr = &leader->followers;
    int count = arr->count;
    if (count == 4) {  // Full!
        return 1;
    }
    count++;
    for (int i = 0; i < 4; i++) {  // Find open space
        if (arr->locations.a[i] == NULL) {
            arr->locations.a[i] = location;
            arr->sizes.a[i] = size;
            location->flags &= ~CHUNK_LEADER;  // clear leader flag to designate follower
            location->size = size;
            location->follower.index = i;
            location->follower.leader = leader;
            return 0;
        }
    }   
    return 1; 
}

inline void copy_leader_links(LeaderTail *dst, LeaderTail *src) {
    dst->next = src->next;
    dst->prev = src->prev;
}

/**
 * Remove a leader from the free list.
 */
inline void pop_leader(LeaderTail *leader) {
    if (leader->followers.count == 0) {  // No children, just remove the leader
        LeaderTail *next = leader->next;
        LeaderTail *prev = leader->prev;
        next->prev = prev;
        prev->next = next;
        return;
    }

    // There are children, the new leader is element 0
    Chunk *replacement_chunk = leader->followers.locations.a[0];
    LeaderTail *replacement = &replacement_chunk->leader;
    replacement_chunk->flags |= CHUNK_LEADER;
    copy_leader_links(replacement, leader);
    copy_chunklist4(&replacement->followers, &leader->followers);
    replacement->followers.locations.a[0] = NULL;    
}

/**
 * Remove a follower from the free list.
 */
inline void pop_follower(FollowerTail *follower) {
    LeaderTail *leader = follower->leader;
    ChunkList4 *followers = &leader->followers;
    followers->count--;
    followers->locations.a[follower->index] = NULL;
}

inline void pop_chunk_from_free_list(Chunk *chunk) {
    if (chunk->flags & CHUNK_LEADER) {
        pop_leader(&chunk->leader);
    } else {
        pop_follower(&chunk->follower);
    }
}

inline void add_after_chunk(Chunk *chunk, Chunk *new_chunk, int size, char occupied) {
    if (chunk->flags & CHUNK_MANAGER) {
        insert_new_chunk(chunk, size, new_chunk, occupied);
    } else {
        insert_new_chunk(chunk->employee.manager, size, new_chunk, occupied);
    }
}


#endif