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

typedef struct Chunk;

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
    __m256i reg[2];
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
} Employee;

/**
 * The tail of a free chunk that contains a list of other free chunks.
 */
typedef struct LeaderTail {
    LeaderTail *prev;
    LeaderTail *next;
    ChunkList4 tail;
} LeaderTail;

/**
 * The tail of a free chunk that is listed in a Leader chunk.
 */
typedef struct FollowerTail {
    LeaderTail *leader;
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
    union {
        LeaderTail leader;
        FollowerTail follower;
    };
} Chunk;

void copy_chunklist4(ChunkList4 *dst, ChunkList4 *src) {
    __m256i a, b;
    a = src->reg[0];
    b = src->reg[0];
    dst->reg[0] = a;
    dst->reg[1] = b;
}

inline __m128i arr4i_perm_insert(__m128i arr4i, int index) {
    switch (index) {
        case 0:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_CBAD);
        case 1:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_CBDA);
        case 2:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_CDBA);
        default:
            return arr4i;
    }
}

inline __m256i arr4p_perm_insert(__m256i arr4p, int index) {
    switch (index) {
        case 0:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_CBAD);
        case 1:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_CBDA);
        case 2:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_CDBA);
        default:
            return arr4p;
    }
}

inline __m128i arr4i_perm_remove(__m128i arr4i, int index) {
    switch (index) {
        case 0:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_ADCB);
        case 1:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_BDCA);
        case 2:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_CDBA);
        default:
            return arr4i;
    }
}

inline __m256i arr4p_perm_remove(__m256i arr4p, int index) {
    switch (index) {
        case 0:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_ADCB);
        case 1:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_BDCA);
        case 2:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_CDBA);
        default:
            return arr4p;
    }
}

inline __m128i arr4i_perm_remove2(__m128i arr4i, int index) {
    switch (index) {
        case 0:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_BADC);
        case 1:
            return (__m128i)_mm_permute_ps((__m128)arr4i, _MM_PERM_CBDA);
        case 2:
            return arr4i;
        default:
            return arr4i;
    }
}

inline __m256i arr4p_perm_remove2(__m256i arr4p, int index) {
    switch (index) {
        case 0:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_BADC);
        case 1:
            return _mm256_permute4x64_epi64(arr4p, _MM_PERM_CBDA);
        case 2:
            return arr4i;
        default:
            return arr4p;
    }
}

void insert_at(Manager *manager, int index, int size, Chunk *location) {
    Arr4i sizes;
    Arr4p locations;
    int count = manager->employees.count;
    sizes._reg = manager->employees.sizes._reg;
    locations._reg = manager->employees.locations._reg;

    if (count == 4) {  // Space exceeded, make a new node
        int size = sizes.a[3];
        Chunk *next = locations.a[3];
        
        manager->next = next;
        next->manager.prev = (Chunk*)((char*)manager - 4);
        next->flags |= CHUNK_MANAGER;
        next->manager.employees.count = 0;
    } else {
        count++;
    }
    locations._reg = arr4p_perm_insert(locations._reg, index);
    sizes._reg = arr4i_perm_insert(sizes._reg, index);

    locations.a[index] = location;
    sizes.a[index] = size;

    manager->employees.count = count;
    manager->employees.locations._reg = locations._reg;
    manager->employees.sizes._reg = sizes._reg;
    location->employee.manager = manager;
    location->flags &= ~CHUNK_MANAGER;  // Clear manager flag to designate that it's an employee
}

void remove_at(Manager *manager, int index) {
    Arr4i sizes;
    Arr4p locations;
    int count = manager->employees.count;
    sizes._reg = manager->employees.sizes._reg;
    locations._reg = manager->employees.locations._reg;


}


#endif