#include <stdio.h>

int div(int x, int y) {
    int cnt = 1;
    while (x > y) {
        cnt <<= 1;
        y <<= 1;
    }
    int out = 0;
    while (1) {
        cnt >>= 1;
        y >>= 1;
        if (y < x) {
            out |= cnt;
            x -= y;
        }
        if (cnt == 0) {
            break;
        }
    }
    return out;
}

int main() {
    printf("starting\n");
    int quot = div(932548, 8430);
    printf("%d\n", quot);
    return 0;
}
