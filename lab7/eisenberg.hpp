#include <sys/mman.h>

enum pstate {
    IDLE, WAITING, ACTIVE
};

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
 * Initialize prior to forking.
 */
class Eisenberg {
private:
    pstate *flags;
    const int n;
    int turn = 0;
    int index;
    int i;
public:
    Eisenberg(int n) : n(n) {
        flags = (pstate*)mmap(
            nullptr,
            sizeof(pstate) * n,
            PROT_READ | PROT_WRITE,
            MAP_SHARED | MAP_ANONYMOUS,
            -1,
            0
        );
    }

    void set_i(int i) {
        this->i = i;
    }

    EisenbergLock lock() {
        do {
            flags[i] = WAITING;
            index = turn;
            while (index != i) {
                index = flags[index] != IDLE
                    ? turn 
                    : index = (index + i) % n;
            }

            flags[i] = ACTIVE;
            index = 0;
            while (index < n && index == i || flags[index] != ACTIVE) {
                index++;
            }
        } while (!(index >= n && turn == i || flags[turn] == IDLE));
        turn = i;
        return EisenbergLock(this);
    }

    void unlock() {
        index = (turn + 1) % n;
        while (flags[index] == IDLE) {
            index = (index + 1) % n;
        }

        turn = index;
        flags[index] = IDLE;
    }

    ~Eisenberg();
};

EisenbergLock::EisenbergLock(Eisenberg *parent)
    : parent(parent) 
{ }

EisenbergLock::~EisenbergLock() {
    parent->unlock();
}