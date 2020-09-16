// Unit tests!

#include <assert.h>
#include "./malloc.c"

/**
 * Sets the memory into a state such that it is completely free.
 */
void reset_memory() {
    brk(first_chunk);
}

int get_n_chunks() {
    if (is_heap_empty()) return 0;
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

chunkhead* get_chunk_of(byte *p) {
    return (chunkhead*) (p - sizeof(chunkhead));
}

void assert_no_dangling_pointers() {
    if (is_heap_empty) return;
    chunkhead *chunk = first_chunk;
    while (chunk != NULL) {
        assert(chunk->next < sbrk(0));
        assert(chunk->prev >= first_chunk);
        chunk = chunk->next;
    }
}

void test_sequential_malloc_and_free() {
    byte *a[5];
    for (int i = 0; i < 5; i++) {
        a[i] = mymalloc(1000);
    }
    
    assert(get_n_chunks() == 5);
    assert(get_managed_chunk(0)->info);
    assert(get_managed_chunk(1)->info);
    assert(get_managed_chunk(2)->info);
    assert(get_managed_chunk(3)->info);
    assert(get_managed_chunk(4)->info);
    assert_no_dangling_pointers();

    for (int i = 0; i < 5; i++) {
        myfree(a[i]);
    }

    assert_no_dangling_pointers();
    assert(first_chunk == sbrk(0));
    assert(get_n_chunks() == 0);
}

void test_best_fit() {
    byte *a[5];
    a[0] = mymalloc(9000);
    a[1] = mymalloc(54000);
    a[2] = mymalloc(12000);
    a[3] = mymalloc(3000);
    a[4] = mymalloc(9000);
    assert(get_n_chunks() == 5);

    myfree(a[1]);
    myfree(a[3]);
    assert(get_n_chunks() == 5);

    byte *small = mymalloc(1000);
    assert(get_n_chunks() == 5);
    assert(small == a[3]);
}

void test_merge_1() {
    byte *a[4];

    a[0] = mymalloc(1000);
    a[1] = mymalloc(1000);
    a[2] = mymalloc(1000);
    a[3] = mymalloc(1000);

    assert_no_dangling_pointers();
    assert(get_n_chunks() == 4);
    
    myfree(a[3]);
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 3);

    myfree(a[1]);
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 3);
    
    a[3] = mymalloc(1000);
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 3);
}

void test_merge_2() {
    byte *a[5];
    for (int i = 0; i < 5; i++) {
        a[i] = mymalloc(1000);
    }

    myfree(a[1]);
    myfree(a[3]);
    assert(get_n_chunks() == 5);
    chunkhead *dbg = get_chunk_of(a[4]);
    assert((char*)dbg + dbg->size == sbrk(0));

    myfree(a[2]);
    assert(get_n_chunks() == 3);
    dbg = get_chunk_of(a[4]);
    assert((char*)dbg + dbg->size == sbrk(0));

    myfree(a[4]);
    assert(get_n_chunks() == 1);
    assert(a[1] - sizeof(chunkhead) == sbrk(0));
}

void test_merge_prev() {
    byte *a[5];
    for (int i = 0; i < 5; i++) {
        a[i] = mymalloc(1000);
    }

    myfree(a[1]);
    assert(get_n_chunks() == 5);
    chunkhead *dbg = get_chunk_of(a[4]);
    assert((char*)dbg + dbg->size == sbrk(0));

    myfree(a[2]);
    assert(get_n_chunks() == 4);
    dbg = get_chunk_of(a[4]);
    assert((char*)dbg + dbg->size == sbrk(0));
    dbg = get_chunk_of(a[0]);
    assert(dbg->size == PAGESIZE);
    assert(dbg->info == 1);
    dbg = get_chunk_of(a[1]);
    assert(dbg->size == PAGESIZE * 2);
    assert(dbg->info == 0);
    dbg = get_chunk_of(a[3]);
    assert(dbg->info == 1);
    assert(dbg->size == PAGESIZE);

}

void reset_with_asserts() {
    reset_memory();
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 0);
}

int main() {
    test_sequential_malloc_and_free();
    reset_with_asserts();

    test_best_fit();
    reset_with_asserts();

    test_merge_1();
    reset_with_asserts(); 
    
    test_merge_2();
    reset_with_asserts();

    test_merge_prev();
    reset_with_asserts();
    
    return 0;
}