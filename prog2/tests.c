// Unit tests!

#include <assert.h>
#include "./malloc.c"

/**
 * Sets the memory into a state such that it is completely free.
 */
void reset_memory() {
    brk(first_chunk);
}

int main() {
    byte *a[100];
    for (int i = 0; i < 5; i++) {
        a[i] = mymalloc(1000);
    }
    analyse();

    reset_memory();

    return 0;
}