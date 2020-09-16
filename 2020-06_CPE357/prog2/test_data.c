#include "data.h"
#include "stdio.h"
#include <assert.h>


void test_chunksize_fits() {
    Chunk chunk;
    int used_header = (char*)&chunk.leader - (char*)&chunk;
    printf("Used size: %d / Free size: %d\n", used_header, (int) sizeof(Chunk));
    assert(used_header <= 128);
}

int main() {
    test_chunksize_fits();
}
