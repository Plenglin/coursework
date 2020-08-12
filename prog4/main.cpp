#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#include "./stack.hpp"

#define STATUS_ALIVE 0
#define STATUS_UNKNOWN 1
#define STATUS_QUIT 2


#define MAX_CHILD_PROCS 10


struct SearchParams {
    bool subdirs;
    char ext[256];
    char name[256];
};
class Task {
public:
    char path[256];
};

struct Result {
    char path[256];
};
typedef SharedStack<Task*> TaskQueue;


enum ProcessStatus {
    PROC_WAITING, PROC_ACTIVE, PROC_TERMINATING, PROC_HALTED
};

void copy_path(char *dst, char *folder, char *child) {
    strcpy(dst, folder);
    int len = strlen(dst);
    if (dst[len - 1] != '/') {
        dst[len++] = '/';
    }
    strcpy(dst + len, child);
}

template <class DirsCollection, class MatchesCollection, class TMutex>
int search_directory(char *path, SearchParams *params, DirsCollection *tasks, MatchesCollection *matches, TMutex matches_mutex) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        return 1;
    }
    char buf[256];
    for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
        if (dirent->d_type == DT_DIR) { // Is a directory?
            if (tasks != NULL && strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, "..")) {  // Not a fake directory?
                Task *new_task = malloc_shared<Task>(1);
                copy_path(new_task->path, path, dirent->d_name);
                tasks->push(new_task);
            }
            continue;
        }

        if (params->name[0] || params->ext[0]) {  // Filter strings are not empty strings?
            // Prepare for filtering

            strcpy(buf, dirent->d_name);
            char *ext = parse_name_ext(buf); 
            if (params->name[0] && strcmp(buf, params->name)) {  // No name match?
                continue;
            }           
            if (params->ext[0] && strcmp(ext, params->ext)) {  // No extension match?
                continue;
            }
        }

        Result *result = malloc_shared<Result>(1);
        copy_path(result->path, path, dirent->d_name);
        {
            auto lock = matches_mutex.lock();
            matches->push(result);
        }
    }
    closedir(dir);
    return 0;
}

void worker_sig_handler(int sig) {
    // noop
}

struct SharedProcessData {
    ProcessStatus status;
    char current_dir[256];
    bool *active_flag;
};

/**
 * Handles the interfacing between the monitor process and a SINGLE child process for recursive searches.
 */
class ProcessController {
    int i;
    SharedProcessData *shared;
    Mutex shared_mutex;
    int child_pid;
    
    Mutex results_mutex;
    SearchParams *params;
    TaskQueue *task_queue;
    Stack<Result*> *results;
public: 
    ProcessController(SearchParams *params, int i, TaskQueue *task_queue, Stack<Result*> *results, Mutex results_mutex, bool *active_flag) : 
        shared_mutex(2), 
        i(i),
        results(results),
        results_mutex(results_mutex),
        task_queue(task_queue),
        params(params)
    {
        shared = malloc_shared<SharedProcessData>();
        shared->status = PROC_ACTIVE;
        shared->active_flag = active_flag;
        *active_flag = false;

        int copi[2], cipo[2];
        pipe(copi);
        pipe(cipo);
        child_pid = fork();
        if (child_pid) {
            // parent
            shared_mutex.set_i(0);
        } else {
            // child;
            shared_mutex.set_i(1);

            task_queue->set_proc_i(i);
            results_mutex.set_i(i);
            do_child();
            exit(0);
        }
    }

    void do_child() {
        signal(SIGTERM, worker_sig_handler);

        Task* tasks[10];  // We want a buffer so that we aren't spending a ton of time in the mutex.
        while (get_status() != PROC_TERMINATING) {
            {
                auto lock = shared_mutex.lock();
                shared->status = PROC_WAITING;
            }
            int n_tasks = task_queue->pop(1, 10, tasks);
            {
                auto lock = shared_mutex.lock();
                shared->status = PROC_ACTIVE;
            }
            
            *shared->active_flag = true;
            for (int i = 0; i < n_tasks; i++) {
                Task *task = tasks[i];
                {
                    auto lock = shared_mutex.lock();
                    strcpy(shared->current_dir, task->path);
                }
                search_directory(task->path, params, task_queue, results, results_mutex);
                free_shared(task);
            }
            *shared->active_flag = false;
        }

        {
            auto lock = shared_mutex.lock();
            shared->status = PROC_HALTED;
        }
        return;
    }

    ProcessStatus get_status() {
        auto lock = shared_mutex.lock();
        return shared->status;
    }

    void end_child_process() {
        {
            auto lock = shared_mutex.lock();
            shared->status = PROC_TERMINATING;
        }
        kill(child_pid, SIGTERM);
    }

    ~ProcessController() {
        kill(child_pid, SIGTERM);
        free_shared(shared);
    }    
};

/**
 * Handles a collection of processes for recursive searches. Begins searching on allocation.
 */
template <int max_n>
class ProcessPool {
    ProcessController* procs[max_n] = {nullptr};
    Stack<Result> *results;
    Mutex results_mutex;
    TaskQueue tasks;
    int n = 0;
public:
    ProcessPool() {
        for (int i = 0; i < max_n; i++) {

        }
    }

    void search(char *dir) {
        //malloc(strlen())
    }

    ~ProcessPool() {

    }
};

int do_list(char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        printf("Directory not available: %s\n", path);
        return -1;
    }
    for (struct dirent *ent = readdir(dir); ent != NULL; ent = readdir(dir)) {
        printf("%s\n", ent->d_name);
    }
    closedir(dir);
    return 0;
}

void print_prog_status(int status, char *path) {
    if (status) {  // Error
        write(0, "\033[0;31m", 8);  // Red
    } else {  // Normal
        write(0, "\033[0;34m", 8);  // Blue
    }
    const_print("findstuff \033[0;36m");
    write(0, path, strlen(path));
    const_print("\033[0m$ ");
    fflush(0);
}

int do_find_cmd(char *term, char *ext, bool subdirs) {
    
}

int process_cmd(char *path, char *input) {
}

void do_parent(int parent_pid) {
    char path[1000];
    char input[1000];
    //signal(SIGUSR1, child_isr);

    int status = 0;

    while (1) {
        getcwd(path, 1000);
        print_prog_status(status, path);
        fgets(input, 999, stdin);
        // Trim the trailing newline
        int len = strlen(input);
        if (len > 0) {
            input[len - 1] = 0;
        }
        status = process_cmd(path, input);
    }
}

int main() {
    Stack<Result*> results;
    Stack<Task*> tasks;
    SearchParams params;
    NullMutex mutex;
    strcpy(params.ext, "h");
    strcpy(params.name, "");
    params.subdirs = false;
    search_directory<Stack<Task*>, Stack<Result*>, NullMutex>("prog2", &params, &tasks, &results, mutex);
    printf("files\n");
    for (auto iter = results.start(); iter != results.end(); iter++) {
        printf("%s\n", (*iter)->path);
    }
    printf("dirs\n");
    for (auto iter = tasks.start(); iter != tasks.end(); iter++) {
        printf("%s\n", (*iter)->path);
    }
    return 0;
}
