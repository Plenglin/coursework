#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <immintrin.h>

#define COUNT (1 << 24)

typedef struct Node {
    Node *next;
    int val;
} Node;

char a[COUNT];
char b[COUNT];

void test1() {
    for (int i = 0; i < COUNT; i++) {
        b[i] = a[i];
    }
}

void test2() {
    for (int i = 0; i < COUNT/32; i+=32) {
        *((__m256*)(b + i)) = *((__m256*)(a+i));
    }
}

typedef union Nums {
    long nums[4];
    __m256i _reg;
} Nums;

void print_nums(Nums *nums) {
    for (int i = 0; i < 4; i++) {
        printf("%ld ", nums->nums[i]);
    }
    printf("\n");
}

int main() {
    Nums a;
    a.nums[0] = 0;
    a.nums[1] = 1;
    a.nums[2] = 2;
    a.nums[3] = 3;

    print_nums(&a);
    __m256i ra = _mm256_permute4x64_epi64(a._reg, _MM_PERM_DDCB);
    ra = _mm256_set1_epi64x();
    reg = _mm256_and_si256()
    a._reg = _mm256_permute4x64_epi64(a._reg, _MM_PERM_DDCB);
    a._reg = _mm256_permute4x64_epi64(a._reg, _MM_PERM_DDCB);
    print_nums(&a);

    clock_t t0 = clock();
    test1();
    clock_t t1 = clock();
    test2();
    clock_t t2 = clock();

    printf("%ld %ld", t1 - t0, t2 - t1);
}