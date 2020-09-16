#include <stdio.h>

int main() {
    int height;
    scanf("%i", &height);
    
    for (int i = 0; i < height; i++) {
        int width = 2 * i + 1;
        int padding = height - i;
        for (int j = 0; j < padding; j++) {
            printf(" ");
        }
        for (int j = 0; j < width; j++) {
            printf("*");
        }
        printf("\n");
    }

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < height; j++) {
            printf(" ");
        }
        printf("*\n");
    }
        
    return 0;
}

