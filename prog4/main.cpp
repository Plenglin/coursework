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



struct FileResult {
    FileResult *next = nullptr;
    char path[256];

    static FileResult* create() {
        auto *list = malloc_shared<FileResult>();
        list->next = nullptr;
        return list;
    }
};

/**
 * A collection containing the results of a directory search.
 */
struct DirectoryResult {
    /**
     * Siblings of this.
     */
    DirectoryResult *next;
    /**
     * Files in this.
     */
    FileResult *files;
    /**
     * Appended to by this object.
     */
    DirectoryResult *subdirs;

    static DirectoryResult* create() {
        auto *list = malloc_shared<DirectoryResult>();
        list->next = nullptr;
        list->files = nullptr;
        list->subdirs = nullptr;
        return list;
    }
};

/**
 * Appends a new node to the object and returns it.
 */
template <class Node>
Node* append(Node *list, Node *item) {
    if (list == nullptr) return item;
    item->next = list;
    return item;
}

/**
 * Describes a task where you scan a SINGLE directory.
 */
struct RecursiveSearchParams {
    /**
     * The directory result you want to set the files and subdirs of. Next will not be modified.
     */
    DirectoryResult *results;
    /**
     * Path to look in
     */
    char *path;
    /**
     * File extension
     */
    char *ext;
    /**
     * File name
     */
    char *name;
    /**
     * Next item in the task list.
     */
    RecursiveSearchParams *next;

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

    static RecursiveSearchParams* create() {
        auto *task = malloc_shared<RecursiveSearchParams>();
        task->next = nullptr;
        return task;
    }
};

int build_path(char *dst, char *folder, char *child) {
    strcpy(dst, folder);
    int len = strlen(dst);
    if (dst[len - 1] != '/') {
        dst[len++] = '/';
    }
    strcpy(dst + len, child);
    return len + strlen(dst + len);
}

class ProcessPool {
    struct Shared {
        int available_procs;
    };
    Mutex mutex;
    const int original_max_procs;
    int max_procs;
    int *child_pids;
    Shared *shared;
    RecursiveSearchParams *tasks;
public:
    ProcessPool(int max_procs) : 
        original_max_procs(max_procs), 
        max_procs(max_procs),
        mutex(max_procs + 1), 
        tasks(nullptr) 
    {
        shared = malloc_shared<Shared>();
        child_pids = new int[max_procs];
    }

    int search_directory_recursive_fork(RecursiveSearchParams params) {
        RecursiveSearchParams *tasks = nullptr;

        {
            DIR *dir = opendir(params.path);
            if (dir == NULL) {
                return 1;
            }

            // Scan this directory.
            FileResult *files = nullptr;
            DirectoryResult *directories = nullptr;
            char buf[4096];
            for (struct dirent *dirent = readdir(dir); dirent != NULL; dirent = readdir(dir)) {
                if (dirent->d_type == DT_DIR) { // Is a directory?
                    if (strcmp(dirent->d_name, ".") && strcmp(dirent->d_name, "..")) {  // Not a fake directory?
                        // Add directory to list
                        auto directory = DirectoryResult::create();
                        directories = append(directories, directory);

                        auto task = new RecursiveSearchParams;
                        task->results = directory;
                        task->name = params.name;
                        task->ext = params.ext;
                        int len = build_path(buf, params.path, dirent->d_name);
                        task->path = strcpy(new char[len + 1], buf);
                        tasks = append(tasks, task);
                    }
                    continue;
                }

                // Does the file match the filter?
                if (!params.match(dirent->d_name)) continue;

                // Add file to list
                auto *file = FileResult::create();
                strcpy(file->path, dirent->d_name);
                files = append(files, file);
            }
            closedir(dir);

            // Set the results.
            params.results->files = files;
            params.results->subdirs = directories;
        }

        // Spawn new threads, or scan it ourselves if we can't spawn new threads.
        for (auto *task = tasks; task != nullptr;) {
            auto task_value = *task;
            delete task;
            task = task_value.next;
            
            if (!request_proc(task_value)) {
                search_directory_recursive_fork(task_value);
            }
        }
        
        free_shared<char>(params.path, strlen(params.path) + 1);
        return 0;
    }

    void do_parent() {
        mutex.set_i(0);
        while (shared->available_procs < original_max_procs) {
            auto lock = mutex.lock();
            while (tasks != nullptr) {
                auto task = *tasks;
                free_shared(tasks);
                tasks = task.next;

                auto pid = fork();
                if (pid) {  // parent
                    child_pids[shared->available_procs] = pid;
                } else {  // child
                    search_directory_recursive_fork(task);
                    exit(0);
                }
            }
        }
    }

    /**
     * Attempts to spawn a new thread. If there are too many threads, nothing happens.
     * Returns whether or not a thread was spawned.
     */
    bool request_proc(RecursiveSearchParams task) {
        auto lock = mutex.lock();
        if (shared->available_procs == 0) return false;

        shared->available_procs--;
        auto *pTask = malloc_shared<RecursiveSearchParams>();
        *pTask = task;
        tasks = append(tasks, pTask);
        return true;
    }

    ~ProcessPool() {
        delete child_pids;
        free_shared(shared);
    }
};

int main() {
    ProcessPool pool(2);
    DirectoryResult result;
    RecursiveSearchParams params;
    params.path = ".";
    params.results = &result;
    params.name = "";
    params.ext = "";
    pool.search_directory_recursive_fork(params);
    return 0;
}
