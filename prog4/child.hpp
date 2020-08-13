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
    int write_fd = 0;
    int i;
    bool is_recursive;
    char *path;
    ProcState state = proc_dead;
    Matcher matcher;
};

Mutex stdout_mutex(MAX_CHILD_PROCS + 1);

void print_results(int fd, std::vector<char*> &results) {
    auto lock = stdout_mutex.lock();

    char buf[4000];
    for (auto iter = results.begin(); iter != results.end(); iter++) {
        auto str = *iter;
        int size = sprintf(buf, "%s\n", str);
        write(fd, buf, size + 1);
        delete str;
    }

    fflush(0);
}

void do_child(ProcessInfo *proc_info) {
    std::vector<char*> dirs;
    std::vector<char*> file_results;
    stdout_mutex.set_i(proc_info->i);

    if (proc_info->is_recursive) {
        scan_path_recursive(&proc_info->matcher, proc_info->path, file_results);
    } else {
        scan_path(proc_info->path, &proc_info->matcher, nullptr, file_results);
    }

    print_results(proc_info->write_fd, file_results);
}

#endif // ___CHILD_HPP___