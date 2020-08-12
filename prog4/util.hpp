#if !defined(__UTIL_HPP__)
#define __UTIL_HPP__

#include <string.h>

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
            last_period_offset = i;
        }
    }
    if (last_period_offset == -1) {
        last_period_offset = i;  // end of string, will be an empty string
    } else {
        str[last_period_offset] = 0;
        last_period_offset++;
    }
    return str + last_period_offset;
}


int build_path(char *dst, char *folder, char *child) {
    strcpy(dst, folder);
    int len = strlen(dst);
    if (dst[len - 1] != '/') {
        dst[len++] = '/';
    }
    strcpy(dst + len, child);
    return len + strlen(dst + len);
}

template <class T>
struct LinkedList {
    LinkedList *next;
    T value;
};

/**
 * Appends a new node to the object and returns it.
 */
template <class T>
LinkedList<T>* append(LinkedList<T> *list, LinkedList<T> *item) {
    if (list == nullptr) return item;
    item->next = list;
    return item;
}

/**
 * Describes a task where you scan a SINGLE directory.
 */
struct Matcher {
    /**
     * File extension
     */
    char ext[512];
    /**
     * File name
     */
    char name[512];

    bool match(char *filename) {
        char buf[256];
        if (this->name[0] || this->ext[0]) {  // Filter strings are not empty strings?
            // Prepare for filtering

            strcpy(buf, filename);
            char *ext = parse_name_ext(buf); 
            if (this->name[0] && strcmp(buf, this->name)) {  // No name match?
                return false;
            }           
            if (this->ext[0] && strcmp(ext, this->ext)) {  // No extension match?
                return false;
            }
        }
        return true;
    }
};

char* read_sized_str(int fd) {
    int size;
    read(fd, &size, sizeof(int));
    char* str = new char[size + 1];
    str[size] = 0;
    read(fd, str, size);
    return str;
}

void write_sized_str(int fd, char *str) {
    int size = strlen(str);
    write(fd, &size, sizeof(int));
    write(fd, str, size);
}

#endif // __UTIL_HPP__
