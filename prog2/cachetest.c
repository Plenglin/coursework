#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>


typedef struct Node {
    Node *next;
    int val;
} Node;

int arr[10000];

void test1() {
    for (int i = 0; i < 10000; i++) {
        arr[i] = 32;
    }
}

void test2() {
    Node *foo = NULL;
    for (int i = 0; i < 10000; i++) {
        Node *nnode = (Node*) malloc(sizeof(Node));
        nnode->next = foo;
        foo = nnode;
        foo->val = 32;
    }
}

int main() {
    clock_t t0 = clock();
    test1();
    clock_t t1 = clock();
    test2();
    clock_t t2 = clock();

    printf("%ld %ld", t1 - t0, t2 - t1);
}