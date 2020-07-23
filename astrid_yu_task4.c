#include <stdlib.h>     // srand, rand 
#include <time.h>       // time 
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

#define ARR_LENGTH 100000
#define ARR_SIZE sizeof(int) * ARR_LENGTH


void count_3s(int *arr, int arr_length, int *result) {
    int count = 0;
    for (int i = 0; i < arr_length; i++) {
        if (arr[i] == 3) count++;
    }
    *result = count;
}

void init_array(int *arr, int length) {
    for (int i = 0; i < length; i++) {
        arr[i] = rand() % 10;
    }
}

int main () {
    srand(time(NULL)); // initialize random seed. That's important and comes right at the start of the main function and only once, never again.
    
    int *arr = mmap(
        NULL,
        ARR_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    int *child_result = mmap(
        NULL,
        sizeof(int),
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    int parent_result;

    init_array(arr, ARR_LENGTH);

    if (fork() == 0) { // Child
        count_3s(arr, ARR_LENGTH / 2, child_result);
        exit(0);
    } else {
        count_3s(arr + ARR_LENGTH / 2, ARR_LENGTH / 2, &parent_result);
        wait(0);
    }

    printf("Child result: %d\n", *child_result);
    printf("Parent result: %d\n", parent_result);

    int total_result = parent_result + *child_result;
    printf("Total result: %d\n", total_result);

    // DISPOSE!!!
    munmap(arr, ARR_SIZE);
    munmap(child_result, sizeof(int));
}
