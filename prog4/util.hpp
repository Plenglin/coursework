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

enum MatcherFlags {
    by_ext = 1,
    by_name = 2
};

/**
 * Describes a task where you scan a SINGLE directory.
 */
struct Matcher {
    int filter = 0;
    /**
     * File extension
     */
    char ext[512] = "";
    /**
     * File name
     */
    char name[512] = "";

    bool match(char *filename) {
        if (!filter) return true;

        char buf[256];
        strcpy(buf, filename);
        char *ext = parse_name_ext(buf); 
        if ((filter & by_name) && strcmp(buf, this->name)) {  // No name match?
            return false;
        }           
        if ((filter & by_ext) && strcmp(ext, this->ext)) {  // No extension match?
            return false;
        }
        return true;
    }
};

void scan_path(char *path, Matcher *matcher, std::vector<char*> &dirs, std::vector<char*> &file_results) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return;
    }

    // Scan this directory.
    char buf[4096];
    for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
        if (dirent->d_type == DT_DIR) { // Is a directory?
            if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, "..")) {  // Not a fake directory?
                // Tell the parent about subdirectory
                int len = build_path(buf, path, dirent->d_name);
                auto str = strcpy(new char[len + 1], buf);
                str[len] = 0;
                dirs.push_back(str);
            }
            continue;
        }

        // Does the file match the filter?
        if (!matcher->match(dirent->d_name)) continue;

        // Tell the parent about the file match
        int len = build_path(buf, path, dirent->d_name);
        auto str = strcpy(new char[len + 1], buf);
        str[len] = 0;
        file_results.push_back(str);
    }
    closedir(dir);
}

std::vector<char*> scan_path_recursive(Matcher *matcher, char *start_path) {
    std::vector<char*> stack;
    std::vector<char*> results;

    stack.push_back(start_path);
    while (!stack.empty()) {
        auto path = stack.back();
        stack.pop_back();
        scan_path(path, matcher, stack, results);
        delete path;
    }

    return results;
}

#endif // __UTIL_HPP__
