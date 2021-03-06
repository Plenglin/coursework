#include <time.h>
#include <stdlib.h>
#include <stdlib.h>
#include <unistd.h>

#include "./malloc.c"


int main() {
    unsigned char *a[100];
    clock_t ca, cb;
    ca = clock();
    for (int i = 0; i < 100; i++)
        a[i] = mymalloc(1000);
    for (int i = 0; i < 90; i++)
        myfree(a[i]);
    myfree(a[95]);
    a[95] = mymalloc(1000);
    for (int i = 90; i < 100; i++)
        myfree(a[i]);
    cb = clock();
    printf("\nduration: %lf\n", (double)(cb - ca));
}