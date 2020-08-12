#if !defined(__UTIL_HPP__)
#define __UTIL_HPP__

#include "./lamport.hpp"
#define Mutex Lamport

template <class T>
T* malloc_shared(int n = 1) {
    return (T*)mmap(nullptr, sizeof(T) * n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

template <class T>
void free_shared(T* ptr, int n = 1) {
    munmap(ptr, sizeof(T) * n);
}

#define const_print(str) write(0, str, sizeof(str) - 1)

/**
 * Takes a file name and splits it into name and extension. 
 * Modifies the string passed in. Returns the extension string.
 */
char* parse_name_ext(char *str) {
    int last_period_offset = -1;
    int i;
    for (i = 0; str[i] != 0; i++) {
        if (str[i] == '.') {
            last_period_offset = i + 1;
        }
    }
    if (last_period_offset == -1) {
        last_period_offset = i;  // end of string, will be an empty string
    }
    return str + last_period_offset;
}

/**
 * Acts like a normal mutex, but it doesn't do anything.
 */
class NullMutex {
public:
    NullMutex() {}
    int lock() {
        return 0;
    }
    ~NullMutex() {}
};


#endif // __UTIL_HPP__
