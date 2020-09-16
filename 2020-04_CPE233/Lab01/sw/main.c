#include <stdio.h>

int main() {

    // Declare memory
    int mem[16];
   
    // Accumulator variables
    int prev = 0;
    int sum = 1;

    // Generate fibonacci and write to memory
    for (int i = 0; i < 16; i++) {
        mem[i] = sum;
        int tmp = prev;
        prev = sum;
        sum += tmp;
    }

    // Read from memory sequentially, displaying address
    for (int i = 0; i < 16; i++) {
        printf("%d\t%d\n", i, mem[i]);
    }

    return 0;
}
