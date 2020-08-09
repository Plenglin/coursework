#if !defined(__UTIL_HPP__)
#define __UTIL_HPP__

#include "./eisenberg.hpp"
#define Mutex Eisenberg

template <class T>
T* malloc_shared(int n = 1) {
    return (T*)mmap(nullptr, sizeof(T) * n, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

template <class T>
void free_shared(T* ptr, int n = 1) {
    munmap(ptr, sizeof(T) * n);
}

#define const_print(str) write(0, str, sizeof(str) - 1)


#endif // __UTIL_HPP__
