#include <stdlib.h>
#include <sys/mman.h>

enum pstate {
    IDLE, WAITING, ACTIVE
};

class Eisenberg;

/**
 * Convenience class so we can do RAII lock shenanigans
 */
class EisenbergLock {
private:
    Eisenberg *parent;
public:
    EisenbergLock(Eisenberg *parent);
    ~EisenbergLock();
};

/**
 * Implements the Eisenberg & McGuire algorithm. Please initialize prior to forking, and call set_i after forking.
 */
class Eisenberg {
private:
    pstate *flags;
    int *turn;
    const int n;
    int i;
public:
    Eisenberg(int n) : n(n) {
        turn = (int*)mmap(
            nullptr,
            sizeof(int) + sizeof(pstate) * n,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
        );
        flags = (pstate*)(turn + 1);
        for (int i = 0; i < n; i++) {
            flags[i] = IDLE;
        }
        *turn = 0;
    }

    void set_i(int i) {
        this->i = i;
    }

    EisenbergLock lock() {
        int index;
        while (1) {
            flags[i] = WAITING;
            index = *turn;
            while (index != i) {
                index = flags[index] != IDLE
                    ? *turn 
                    : index = (index + i) % n;
            }

            flags[i] = ACTIVE;
            index = 0;
            while (index < n && (index == i || flags[index] != ACTIVE)) {
                index++;
            }
            if (index >= n && (*turn == i || flags[*turn] == IDLE)) break;
        }
        *turn = i;
        return EisenbergLock(this);
    }

    void unlock() {
        int index = (*turn + 1) % n;
        while (flags[index] == IDLE) {
            index = (index + 1) % n;
        }

        *turn = index;
        flags[i] = IDLE;
    }

    ~Eisenberg() {
        if (i == 0) munmap(turn, sizeof(int) + sizeof(pstate) * n);
    }
};

EisenbergLock::EisenbergLock(Eisenberg *parent)
    : parent(parent) 
{ }

EisenbergLock::~EisenbergLock() {
    parent->unlock();
}