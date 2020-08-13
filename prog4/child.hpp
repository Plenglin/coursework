#ifndef ___CHILD_HPP___
#define ___CHILD_HPP___

#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>

#include "./util.hpp"

#define STATUS_ALIVE 0
#define STATUS_UNKNOWN 1
#define STATUS_QUIT 2


#define MAX_CHILD_PROCS 10


enum ProcState {
    proc_dead, proc_active, proc_terminating
};

struct ProcessInfo {
    int pid = 0;
    int i;
    bool is_recursive;
    ProcState state = proc_dead;
    Matcher matcher;
};

Mutex stdin_mutex(MAX_CHILD_PROCS + 1);

void print_results(std::vector<char*> &results) {
    auto lock = stdin_mutex.lock();

    for (auto iter = results.begin(); iter != results.end(); iter++) {
        auto str = *iter;
        printf("%s\n", str);
        delete str;
    }

    fflush(0);
}

void do_child(ProcessInfo *proc_info) {
    std::vector<char*> dirs;
    std::vector<char*> file_results;

    if (proc_info->is_recursive) {
        
    }
}

#endif // ___CHILD_HPP___