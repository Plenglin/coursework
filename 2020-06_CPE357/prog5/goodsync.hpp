#include <iostream>

#define SIGN_MASK 1 << 31


bool cyclic_lt(int a, int b) {
    return a < b || ((a > 0) && (b < 0));
}

void sync(int id, int pc, int *ready) {
    int syncid = ready[pc] + 1;
    ready[id] = syncid;
    int breakout = 0;
    while (1) {
        breakout = 1;
        for (int i = 0; i < pc; i++) {
            if (cyclic_lt(ready[i], syncid)) {
                breakout = 0;
                break;
            }
        }

        if (breakout == 1) {
            ready[pc] = syncid;
            break;
        }
    }
}

int main() {
    
}