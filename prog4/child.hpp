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
    proc_dead = 0, proc_active, proc_terminating
};

struct ProcessInfo {
    int fds[2];
    int pid = 0;
    int i;
    bool is_recursive;
    /**
     * This state indicator is atomic. No mutex needed to access.
     */
    ProcState *state = malloc_shared<ProcState>();
    Matcher matcher;

    void terminate() {
        if (pid && *state) {
            *state = proc_terminating;
            kill(pid, SIGTERM);
            int status;
            waitpid(pid, &status, 0);
            *state = proc_dead;
        }
    }

    ~ProcessInfo() {
        if (pid != 0)
            free_shared(state);
    }
};

Mutex stdin_mutex(MAX_CHILD_PROCS + 1);
int *interrupting_proc_i;
int stdin_save;
bool was_interrupted = false;
bool is_child_printing = false;

void print_results(ProcessInfo *proc, std::vector<char*> &results) {
    auto lock = stdin_mutex.lock();
    *interrupting_proc_i = proc->i;
    kill(getppid(), SIGUSR1);

    char buf[4000];
    int buf_len = sprintf(buf, "\nProcess index %d found %d files:\n", proc->i, results.size());
    write(proc->fds[1], buf, buf_len);

    for (auto iter = results.begin(); iter != results.end(); iter++) {
        auto str = *iter;
        buf_len = sprintf(buf, "%s\n", str);
        write(proc->fds[1], buf, buf_len + 1);
        delete str;
    }

    close(proc->fds[1]);
    while (*interrupting_proc_i != -1);
}

void do_child(ProcessInfo *proc_info) {
    close(proc_info->fds[0]);

    std::vector<char*> dirs;
    std::vector<char*> file_results;
    stdin_mutex.set_i(proc_info->i);
    char buf[] = ".";

    if (proc_info->is_recursive) {
        scan_path_recursive(&proc_info->matcher, buf, file_results);
    } else {
        scan_path(buf, &proc_info->matcher, nullptr, file_results);
    }

    print_results(proc_info, file_results);
    *proc_info->state = proc_dead;
}

void dispatch_proc(ProcessInfo *proc_info) {
    *proc_info->state = proc_active;
    pipe(proc_info->fds);
    
    proc_info->pid = fork();

    if (!proc_info->pid) {
        do_child(proc_info);
        exit(0);
    }
    close(proc_info->fds[1]);
}

ProcessInfo all_procs[MAX_CHILD_PROCS];

void handle_prints(int sig) {
    auto proc = all_procs + *interrupting_proc_i;
    int *fd = proc->fds;
    dup2(fd[0], STDIN_FILENO);
    close(fd[0]);
}

void init_procs() {
    stdin_save = dup(STDIN_FILENO); 
    interrupting_proc_i = malloc_shared<int>();
    *interrupting_proc_i = -1;

    // Copy 
    int fds[2];
    pipe(fds);
    close(fds[0]);
    dup2(STDOUT_FILENO, fds[1]);
    
    signal(SIGUSR1, handle_prints);

    for (int i = 0; i < MAX_CHILD_PROCS; i++) {
        auto proc_info = all_procs + i;
        proc_info->i = i;
        *proc_info->state = proc_dead;
    }
}

void destroy_procs() {
    for (int i = 0; i < MAX_CHILD_PROCS; i++) {
        auto proc_info = all_procs + i;
        proc_info->terminate();
    }
    for (int i = 0; i < MAX_CHILD_PROCS; i++) {
        auto proc_info = all_procs + i;
        waitpid(proc_info->pid, 0, 0);
    }
    free_shared(interrupting_proc_i);
}

ProcessInfo *get_available_proc() {
    for (int i = 0; i < MAX_CHILD_PROCS; i++) {
        auto proc_info = all_procs + i;
        if (!proc_info->pid || *proc_info->state) {
            return proc_info;
        }
    }
    return nullptr;
}


#endif // ___CHILD_HPP___