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


struct Task {
    char *path;
};
typedef SharedStack<Task> TaskQueue;


enum ProcessStatus {
    PROC_IDLE, PROC_ACTIVE
};

struct SharedProcessData {
    int pid;
    ProcessStatus status;
};

/**
 * Handles the interfacing between main process and a SINGLE child process.
 */
class ProcessController {
    SharedProcessData *shared;
    int i;
    
    // Communication pipes. c = controller, p = peripheral
    int copi, cipo;
    Mutex mutex;
public: 
    ProcessController(int i, TaskQueue *queue) : mutex(2), i(i) {
        shared = malloc_shared<SharedProcessData>();
        shared->status = PROC_IDLE;

        int copi[2], cipo[2];
        pipe(copi);
        pipe(cipo);
        int pid = fork();
        if (pid) {
            // parent
            mutex.set_i(0);
            shared->pid = pid;
            this->cipo = cipo[0];
            this->copi = copi[1];
        } else {
            // child;
            mutex.set_i(1);
            this->cipo = cipo[1];
            this->copi = copi[0];
            do_child(queue);
        }
    }

    void do_child(TaskQueue *queue) {
        Task tasks[10];  // We want a buffer so that we aren't spending a ton of time in the mutex.
        while (true) {
            for (int i = 0; i < queue->pop(1, 10, tasks); i++) {
                // Scan dir and push subdirs as necessary
            }
        }
    }

    ~ProcessController() {
        kill(shared->pid, SIGTERM);
        free_shared(shared);
    }    
};

template <int max_n>
class ProcessPool {
    ProcessController* procs[max_n] = {nullptr};
    TaskQueue tasks;
    int n = 0;
public:
    ProcessPool() {
        
    }

    void new_proc() {
        
    }

    void search(char *dir) {
        malloc(strlen())
    }

    ~ProcessPool() {

    }
};

void child_isr(int sig) {
    __message->status = STATUS_ALIVE;
    //kill(__parent_pid, SIGUSR1);  // Respond to parent's interrupt
}

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

int do_cd(char *path) {
    if (chdir(path)) {
        printf("Error changing into %s\n", path);
        return -1;
    }
    return 0;
}

int process_find_cmd() {

}

int process_cmd(char *path, char *input) {

    return do_stat(input);
}

void do_parent(int parent_pid) {
    char path[1000];
    char input[1000];
    signal(SIGUSR1, child_isr);

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

