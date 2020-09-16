#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COUNT 1000000

int* create_initial_array() {
    int *arr = (int*) mmap(
        NULL,
        sizeof(int) * COUNT,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_SHARED,
        -1,
        0
    );
    return arr;
}

void destroy_array(int *arr) {
    munmap(arr, sizeof(int) * COUNT);
}

void process(int *arr, int start, int end) {
    for (int i = start; i < end; i++) {
        arr[i] = i * 5 + i / 2;
    }
}

int get_start(int fork_i, int procs) {
    return fork_i * COUNT / procs;
}

int get_end(int fork_i, int procs) {
    return get_start(fork_i + 1, procs);
}

/**
 * Forks however many times you specify. Outputs the index of your process.
 * 0 is the parent.
 */
int fork_layers(int layers, int i) {
    int index = (i << 1) | !fork();
    if (layers <= 1) {
        return index;
    }
    return fork_layers(layers - 1, index);
}

void do_test(int layers) {
    int *arr = create_initial_array();

    clock_t start_time = clock();
    int procs = 1 << layers;
    int fork_i = fork_layers(layers, 0);
    int start_i = get_start(fork_i, procs);
    int end_i = get_end(fork_i, procs);
    process(arr, start_i, end_i);
    if (fork_i == 0) {
        wait(0);
    } else {
        exit(0);
    }
    clock_t end_time = clock();
    printf("%d procs done in %ld\n", procs, end_time - start_time);
    for (int i = 0; i < COUNT; i++) {
        //printf("%d ", arr[i]);
    }
    destroy_array(arr);
}

int main() {
    do_test(1);
    do_test(2);
}