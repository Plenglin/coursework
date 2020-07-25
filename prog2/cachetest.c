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

void print_nums2(__m256i vec) {
    long *nums = (long*) &vec;
    for (int i = 0; i < 4; i++) {
        printf("%lx ", nums[i]);
    }
    printf("\n");
}

int main() {
    __m128i a, b, c, d, e;
    __m256i u, v, x, y, z;
    
    a = _mm_set_epi32(3, 2, 1, 0);
    u = _mm256_set_epi64x(3, 2, 1, 0);
    print_nums(a);
    print_nums2(u);
    printf("\n");
    a = (__m128i)_mm_permute_ps((__m128)a, _MM_PERM_CBAD);
    //u = (__m256i)_mm256_permute_pd((__m256d)u, 0b1001);
    u = _mm256_permute4x64_epi64(u, _MM_PERM_CBAD);
    long *arr = (long*) &u;
    //arr[]
    //((int*)&a)[1] = 5;
    print_nums(a);
    print_nums2(u);
}