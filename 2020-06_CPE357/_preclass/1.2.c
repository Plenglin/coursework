#include <stdio.h>

int main() {
    for (int f = 0; f < 300; f += 20) {
        float c = 5 * (f - 32) / 9;
        printf("%3d %3.6f\n", f, c); 
    }
}
