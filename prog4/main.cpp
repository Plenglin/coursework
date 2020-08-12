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
    char *ext;
};
class Task {
public:
    char path[256];
};

struct Result {
    char *path;
};
typedef SharedStack<Task*> TaskQueue;


enum ProcessStatus {
    PROC_IDLE, PROC_ACTIVE
};

struct SharedProcessData {
    ProcessStatus status;
    char current_dir[256];
};

/**
 * Handles the interfacing between main process and a SINGLE child process for recursive searches.
 */
class ProcessController {
    int i;
    SharedProcessData *shared;
    Mutex shared_mutex;
    int child_pid;
    
    Mutex results_mutex;
    SearchParams *params;
    TaskQueue *queue;
    Stack<Result> *results;
public: 
    ProcessController(SearchParams *params, int i, TaskQueue *queue, Stack<Result> *results, Mutex results_mutex) : 
        shared_mutex(2), 
        i(i),
        results(results),
        results_mutex(results_mutex),
        params(params)
    {
        shared = malloc_shared<SharedProcessData>();
        shared->status = PROC_IDLE;

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

            queue->set_proc_i(i);
            results_mutex.set_i(i);
            do_child_recursive(params, queue, results, results_mutex);
        }
    }

    void do_child_recursive(SearchParams *params, TaskQueue *queue, Stack<Result> *results, Mutex mutex) {
        Task* tasks[10];  // We want a buffer so that we aren't spending a ton of time in the mutex.
        char buf[256];
        while (true) {
            for (int i = 0; i < queue->pop(1, 10, tasks); i++) {
                Task *task = tasks[i];
                DIR *dir = opendir(task->path);
                {
                    auto lock = shared_mutex.lock();
                    strcpy(shared->current_dir, task->path);
                }

                for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
                    if (dirent->d_type == DT_DIR) {
                        Task *new_task = malloc_shared<Task>(1);
                        strcpy(task->path, dirent->d_name);
                        queue->push(task);
                        continue;
                    }

                    // not a directory
                    if (params->ext != nullptr) {
                        strcpy(buf, dirent->d_name);
                        char *ext = parse_name_ext(buf);
                        if (strcmp(ext, params->ext) != 0) {
                            continue;
                        }
                    }

                    Result result;
                    result.path = malloc_shared<char>(strlen(dirent->d_name));
                    strcpy(result.path, dirent->d_name);
                    {
                        auto lock = mutex.lock();
                        results->push(result);
                    }
                }

                closedir(dir);
                free_shared(task);
            }
        }
        exit(0);
    }

    ~ProcessController() {
        kill(child_pid, SIGTERM);
        free_shared(shared);
    }    
};

template <int max_n>
class ProcessPool {
    ProcessController* procs[max_n] = {nullptr};
    Stack<Result> *results;
    Mutex results_mutex;
    TaskQueue tasks;
    int n = 0;
public:
    ProcessPool() {
        
    }

    void new_proc() {
        
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

