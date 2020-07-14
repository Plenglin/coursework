// Unit tests!

#include <assert.h>
#include "./malloc.c"

int main() {
    initialize();
    assert(get_managed_chunk(0)->info == 0);
    assert(get_managed_chunk(0)->size == HEAPSIZE - sizeof(chunkhead));
    assert(get_managed_chunk(1) == NULL);
    
    unsigned char *a, *b, *c;
    
    a = mymalloc(PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(0)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(1)->info == 0);
    assert(get_managed_chunk(2) == NULL);

    b = mymalloc(PAGESIZE * 2 - sizeof(chunkhead));
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(1)->info == 1);
    assert(get_managed_chunk(1)->size == 2 * PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(2)->info == 0);
    assert(get_managed_chunk(3) == NULL);

    c = mymalloc(PAGESIZE - sizeof(chunkhead));

    myfree(b);
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(0)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(1)->info == 0);
    assert(get_managed_chunk(2)->info == 1);
    assert(get_managed_chunk(3)->info == 0);
    assert(get_managed_chunk(4) == NULL);

    myfree(a);
    assert(get_managed_chunk(0)->info == 0);
    assert(get_managed_chunk(0)->size == 3 * PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(1)->info == 1);
    assert(get_managed_chunk(2)->info == 0);
    assert(get_managed_chunk(3) == NULL);

    myfree(c);
    assert(get_managed_chunk(0)->info == 0);
    assert(get_managed_chunk(1) == NULL);
    assert(get_managed_chunk(0)->size == HEAPSIZE - sizeof(chunkhead));

    a = mymalloc(10);
    while (!mymalloc(4 * PAGESIZE));
    myfree(a);
    assert(get_managed_chunk(0)->info == 0);
    assert(get_managed_chunk(-2)->info == 1);
    assert(get_managed_chunk(-2)->size == 5 * PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(-1)->info == 0);

    initialize();

    unsigned char *arr[HEAPSIZE / PAGESIZE];
    for (int i = 0; i < HEAPSIZE / PAGESIZE; i++) {
        arr[i] = mymalloc(10);
    }

    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(-2)->info == 1);
    assert(get_managed_chunk(-2)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(-1)->info == 1);
    assert(mymalloc(10) == NULL);

    myfree(arr[HEAPSIZE / PAGESIZE - 1]);
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(-2)->info == 1);
    assert(get_managed_chunk(-2)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(-1)->info == 0);
    assert(mymalloc(PAGESIZE) == NULL);
    assert(mymalloc(3 * PAGESIZE) == NULL);

    myfree(arr[8]);
    myfree(arr[9]);
    myfree(arr[10]);
    myfree(arr[11]);
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(8)->info == 0);
    assert(get_managed_chunk(8)->size == PAGESIZE * 4 - sizeof(chunkhead));
    assert(get_managed_chunk(9)->info == 1);
    assert(get_managed_chunk(9)->size == PAGESIZE - sizeof(chunkhead));
    assert((unsigned char*)get_managed_chunk(9) == heap_start + PAGESIZE * 12);

    a = mymalloc(3 * PAGESIZE);
    chunkhead *ch1 = get_managed_chunk(8);
    chunkhead *ch2 = get_managed_chunk(9);
    assert(a == arr[8]);
    analyse();
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(8)->info == 1);
    assert(get_managed_chunk(8)->size == PAGESIZE * 4 - sizeof(chunkhead));
    assert(ch2->info == 1);
    assert(get_managed_chunk(9)->size == PAGESIZE - sizeof(chunkhead));

    assert(mymalloc(3 * PAGESIZE) == NULL);
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(8)->info == 1);
    assert(get_managed_chunk(8)->size == PAGESIZE * 4 - sizeof(chunkhead));
    assert(get_managed_chunk(9)->info == 1);
    assert(get_managed_chunk(9)->size == PAGESIZE - sizeof(chunkhead));

    assert(get_managed_chunk(-2)->info == 1);
    assert(get_managed_chunk(-2)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(-1)->info == 0);

    myfree(a);
    mymalloc(PAGESIZE * 2);
    assert(get_managed_chunk(0)->info == 1);
    assert(get_managed_chunk(8)->info == 1);
    assert(get_managed_chunk(8)->size == PAGESIZE * 3 - sizeof(chunkhead));
    assert(get_managed_chunk(9)->info == 0);
    assert(get_managed_chunk(9)->size == PAGESIZE - sizeof(chunkhead));
    assert(get_managed_chunk(10)->info == 1);
    assert(get_managed_chunk(10)->size == PAGESIZE - sizeof(chunkhead));

    return 0;
}