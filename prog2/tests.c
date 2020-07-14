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

chunkhead* get_chunk_of(unsigned char *p) {
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

int main() {
    byte *a[100];
    for (int i = 0; i < 5; i++) {
        a[i] = mymalloc(1000);
    }
    
    assert(get_managed_chunk(0)->info);
    assert(get_managed_chunk(1)->info);
    assert(get_managed_chunk(2)->info);
    assert(get_managed_chunk(3)->info);
    assert(get_managed_chunk(4)->info);
    assert_no_dangling_pointers();

    for (int i = 0; i < 5; i++) {
        myfree(a[i]);
    }
    analyse();

    assert_no_dangling_pointers();
    assert(get_n_chunks() == 0);

    reset_memory();
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 0);

    a[0] = mymalloc(1000);
    a[1] = mymalloc(1000);
    a[2] = mymalloc(1000);
    a[3] = mymalloc(1000);

    assert_no_dangling_pointers();
    assert(get_n_chunks() == 4);
    
    myfree(a[3]);
    assert_no_dangling_pointers();
    assert(get_n_chunks() == 4);
    
    return 0;
}