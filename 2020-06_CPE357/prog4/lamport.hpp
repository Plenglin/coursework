#if !defined(___LAMPORT_HPP___)
#define ___LAMPORT_HPP___

#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "./raii_lock.hpp"

/**
 * Implements the Lamport's Bakery algorithm. Please initialize prior to forking, and call set_i after forking.
 */
class Lamport {
private:
    bool *entering;
    unsigned long *number;
    const int n;
    int i;
public:
    Lamport(int n) : n(n) {
        entering = (bool*)mmap(
            nullptr,
            sizeof(bool) * n,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
        );
        number = (unsigned long*)mmap(
            nullptr,
            sizeof(unsigned long) * n,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
        );

        for (int i = 0; i < n; i++) {
            entering[i] = false;
            number[i] = 0;
        }
    }

    void set_i(int i) {
        this->i = i;
    }

    RAIILock<Lamport> lock() {
        entering[i] = true;
        int max = number[0];
        for (int j = 1; j < n; j++) {
            if (max < number[j]) {
                max = number[j];
            }
        }
        number[i] = max + 1;
        entering[i] = false;
        for (int j = 0; j < n; j++) {
            if (j == i) continue;
            while (entering[j]);
            while (number[j] != 0 && number[j] < number[i]);
        }
        return RAIILock<Lamport>(this);
    }

    void unlock() {
        number[i] = 0;
    }

    ~Lamport() {
        if (i == 0) {
            munmap(entering, sizeof(bool) * n);
            munmap(number, sizeof(unsigned long) * n);
        }
    }
};

#endif // ___LAMPORT_HPP___