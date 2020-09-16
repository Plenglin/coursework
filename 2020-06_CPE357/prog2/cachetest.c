#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <immintrin.h>


void print_nums(__m128i vec) {
    int *nums = (int*) &vec;
    for (int i = 0; i < 4; i++) {
        printf("%x ", nums[i]);
    }
    printf("\n");
}

int main() {
    __m128i a, b, c, d, e;
    
    a = _mm_set_epi32(3, 2, 1, 0);
    print_nums(a);
    printf("\n");
    a = (__m128i)_mm_permute_ps((__m128)a, _MM_PERM_CBAD);
    print_nums(a);
}