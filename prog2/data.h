#ifndef __DATA_H__
#define __DATA_H__
#include <immintrin.h>

#define PAGESIZE 4096

#define CHUNK_USED 1
#define CHUNK_MANAGER 2
#define CHUNK_LEADER 4
#define CHUNK_SENTINEL_LEADER 8

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
 * A array list of 4 chunks, with 2x256 bit register backing
 */
typedef union ChunkList4 {
    struct {
        unsigned int count;
        Arr4i sizes;
        Arr4p locations;
    };
    __m256i _reg[2];
} ChunkList4;

/**
 * A chunk that contains a list of used chunks.
 */
typedef struct Manager {
    unsigned int size;
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
    Chunk *leader;
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
    union {
        Manager manager;
        Employee employee;
    };
    // Everything below this line ONLY EXISTS ON FREE CHUNKS.
    union {
        LeaderTail leader;
        FollowerTail follower;
    };
} Chunk;

inline void copy_chunklist4(ChunkList4 *dst, ChunkList4 *src) {
    long *ad = (long*) dst;
    long *as = (long*) src;
    ad[0] = as[0];
    ad[1] = as[1];
    ad[2] = as[2];
    ad[3] = as[3];
}

inline void copy_chunklist4_upper_to_lower(ChunkList4 *dst, ChunkList4 *src) {
    // Copy 128 bits
    long *adl = (long*)&dst->locations;
    long *asl = (long*)&src->locations;
    adl[0] = asl[2];
    adl[1] = asl[3];

    // Copy 64 bits
    long *ads = (long*)&dst->sizes;
    long *ass = (long*)&src->sizes;
    ads[0] = ass[1];
}

/**
 * Permute locations and size for an insert operation. Count is the NEW arraylist count.
 */
inline void perm_arr4_insert(int index, ChunkList4 *list) {
    // This WOULD be a switch statement that does fancy AVX instructions, but guess not. :(
    int next_i;
    for (int i = list->count - 1; i > index; i = next_i) {
        next_i = i - 1;
        list->locations.a[i] = list->locations.a[next_i];
        list->sizes.a[i] = list->sizes.a[next_i];
    }
}

/**
 * Permute locations and size for a remove operation. Count is the NEW arraylist count.
 */
inline void perm_arr4_remove(int index, int count, Arr4p *locations, Arr4i *sizes) {
    int next_i;
    for (int i = index; i < count; i = next_i) {
        next_i = i + 1;
        locations->a[i] = locations->a[next_i];
        sizes->a[i] = sizes->a[next_i];
    }
}

/**
 * Permute locations and size for a remove 2 items operation. Count is the NEW arraylist count.
 */
inline void perm_arr4_remove2(int index, int count, Arr4p *locations, Arr4i *sizes) {
    //TODO 
}

/**
 * Insert into a single array list, deleting the last item if space exceeded.
 */
inline void insert_arr4(int index, ChunkList4 *list, int size, Chunk *location) {
    // Non-last index?
    if (index < list->count - 1) {
        // Permute the array to shift everything over
        perm_arr4_insert(index, list);
    }
    list->locations.a[index] = location;
    list->sizes.a[index] = size;
}

inline void set_manager_chunk(Chunk *chunk, Chunk *prev, Chunk *next) {
    next->manager.prev = prev;
    next->manager.next = next;
    next->flags |= CHUNK_MANAGER;
    next->manager.employees.count = 0;
}

inline void set_employee_chunk(Chunk *chunk, Chunk *manager, int index) {
    chunk->flags &= ~CHUNK_MANAGER;  // Clear manager flag to designate that it's an employee
    chunk->employee.manager = manager;
}

inline void maybe_set_manager_prev(Chunk *chunk, Chunk *prev) {
    if (chunk) {
        chunk->manager.prev = prev;
    }
}

void insert_at(Chunk* this_chunk, int index, int size, Chunk *location) {
    Arr4i sizes;
    Arr4p locations;
    Manager *manager = &this_chunk->manager;

    // Copy the employees list here
    ChunkList4 employees;
    copy_chunklist4(&employees, &manager->employees);

    if (employees.count == 4) {
        // Max size, promote last node to manager
        int size = sizes.a[3];
        Chunk *next = locations.a[3];  // The new next
        maybe_set_manager_prev(manager->next, next);
        set_manager_chunk(next, this_chunk, manager->next);
        manager->next = next;
    } else {
        employees.count++;
    }
    insert_arr4(index, &employees, size, location);

    // Copy the employees list back
    copy_chunklist4(&manager->employees, &employees);
    set_employee_chunk(location, this_chunk, index);
}

void remove_at(Manager *manager, int index) {
    Arr4i sizes;
    Arr4p locations;
    int count = --manager->employees.count;
    sizes._reg = manager->employees.sizes._reg;
    locations._reg = manager->employees.locations._reg;

    perm_arr4_remove(index, count, &locations, &sizes);
}

void remove2_at(Manager *manager, int index) {
    //TODO specialized function
    remove_at(manager, index + 1);
    remove_at(manager, index);
}

/**
 * Push an element onto the leader node.
 */
int push_to_leader(LeaderTail *leader, int size, Chunk *location) {
    int count = chunk->leader.followers.count;
    for (int i = 0; i < 4; i++) {
        if (leader->followers.locations.a[i] == NULL) {
            leader->followers.locations.a[i] = chunk;
            leader->followers.sizes.a[i] = size;
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
void pop_leader(LeaderTail *leader) {
    LeaderTail *next = leader->next;
    LeaderTail *prev = leader->prev;
    if (leader->followers.count == 0) {  // No children, just remove the group
        next->prev = prev;
        prev->next = next;
        return;
    }

    LeaderTail replacement;

    copy_chunklist4(&replacement.followers, &leader->followers);
    Chunk *dst = replacement.followers.locations.a[0];
    replacement.followers.locations.a[0] = NULL;
    
    dst->flags |= CHUNK_LEADER;
    copy_leader_links(&dst->leader, &replacement);
    copy_chunklist4(&dst->leader.followers, &replacement.followers);
}

void pop_follower(Chunk *chunk, int size) {
    LeaderTail *leader = &chunk->follower.leader->leader;
    leader[]
}



#endif