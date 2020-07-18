#include <time.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

#include "./malloc.c"

#define PERF_TIMES 10000


int main() {
    byte *a[100];
    clock_t ca, cb, cc = 0, cd = 0, ce;
    ca = clock();
    for (int n = 0; n < PERF_TIMES; n++) {
        clock_t lst = clock();
        for (int i = 0; i < 100; i++)
            a[i] = mymalloc(1000);
        cc += clock() - lst;
        for (int i = 0; i < 90; i++)
            myfree(a[i]);
        cd += clock() - lst;
        myfree(a[95]);
        a[95] = mymalloc(1000);
        for (int i = 90; i < 100; i++)
            myfree(a[i]);
    }
    cb = clock();
    printf("\nduration: %lf\n", (double)(cc) / PERF_TIMES / 100);
    printf("\nduration: %lf\n", (double)(cd - cc) / PERF_TIMES / 90);
    printf("\nduration: %lf\n", (double)(cb - ca - cc) / PERF_TIMES / 10);
    printf("\nduration: %lf\n", (double)(cb - ca) / PERF_TIMES);
}