#if !defined(___MANAGER_HPP___)
#define ___MANAGER_HPP___

#include <poll.h>
#include <sys/select.h>
#include <fcntl.h>

#include "./util.hpp"
#include "./stack.hpp"
#include "./worker.hpp"

#define MAX_BUFFERED 1000

enum WorkerInstanceStatus {
    wis_idle, wis_active, wis_halting, wis_dead
};

class WorkerInstance;

class Manager {
private:
    WorkerInstance *workers;
    int n_workers;
    int n_dirs = 0;
    Queue<WorkerInstance*> available;

    Queue<char*> results;
    Stack<char*> directories;
public:
    Manager(int n_workers, char* path);
    void set_matcher(Matcher *matcher);
    void add_directory(char *path);
    void add_file(char *path);
    void run();
};

class WorkerInstance {
    WorkerInstanceStatus status;
    Queue<char*> tasks_buffer;
    int i;
    void receive_change_state();
    void receive_directory();
    void receive_file();
    void receive_finished_scan();
public:
    int mowi, miwo, child_pid, n_processing;
    Manager *manager;
    WorkerInstance();

    int buffered();
    void send_set_matcher(Matcher *matcher);
    void terminate_worker();
    bool assign_worker(char *path);
    void read_messages();
    void write_messages();

    ~WorkerInstance();
};

int WorkerInstance::buffered() {
    return tasks_buffer.size();
}

void WorkerInstance::write_messages() {
    auto type = scan_path;
    while (tasks_buffer.size() > 0 && n_processing < MAX_TASKS) {
        char *path = tasks_buffer.pop();
        n_processing++;
        write(mowi, &type, sizeof(ManagerMessageType));
        write_sized_str(mowi, path);
        delete path;
    }
}

void WorkerInstance::receive_directory() {
    char *path = read_sized_str(miwo);
    manager->add_directory(path);
}

void WorkerInstance::receive_file() {
    char *path = read_sized_str(miwo);
    manager->add_file(path);
}

void WorkerInstance::receive_change_state() {
    WorkerStatus status;
    read(miwo, &status, sizeof(WorkerStatus));
    switch (status) {
        case WorkerStatus::idle:
            this->status = wis_idle;
            break;
        case WorkerStatus::active:
            this->status = wis_active;
            break;
        case WorkerStatus::dead:
            this->status = wis_dead;
            break;
    }
}

WorkerInstance::WorkerInstance() {
    int mowi[2];
    int miwo[2];
    pipe(mowi);
    pipe(miwo);

    this->mowi = mowi[1];
    this->miwo = miwo[0];

    child_pid = fork();
    if (!child_pid) {
        {
            Worker worker(mowi[0], miwo[1]);
            worker.run();
        }
        exit(0);
    }
}

WorkerInstance::~WorkerInstance() {
    terminate_worker();
    close(mowi);
}

void WorkerInstance::send_set_matcher(Matcher *matcher) {
    auto type = set_matcher;
    write(mowi, &type, sizeof(ManagerMessageType));
    write(mowi, matcher, sizeof(Matcher));
}

void WorkerInstance::terminate_worker() {
    auto type = terminate;
    write(mowi, &type, sizeof(ManagerMessageType));
    status = wis_halting;
}

bool WorkerInstance::assign_worker(char *path) {
    if (tasks_buffer.size() >= MAX_BUFFERED) return false;
    tasks_buffer.push(path);
    auto type = scan_path;
    return true;
}

void WorkerInstance::receive_finished_scan() {
    int size;
    read(miwo, &size, sizeof(int));
    n_processing -= size;
}

void WorkerInstance::read_messages() {
    pollfd pollfd;
    while (1) {
        pollfd.events = POLLIN;
        pollfd.fd = miwo;
        poll(&pollfd, 1, 0);
        if (!(pollfd.revents & POLLIN)) {
            return;
        }

        WorkerMessageType type;
        read(miwo, &type, sizeof(WorkerMessageType));
        switch (type) {
            case found_file:
                receive_file();
                break;
            case found_directory: 
            case unprocessed_directory:
                receive_directory();
                break;
            case finished_scan:
                receive_finished_scan();
                break;
            case change_state:
                receive_change_state();
                break;
        }
    } 
}

Manager::Manager(int n_workers, char *path) : 
    n_workers(n_workers), 
    available(n_workers) 
{
    workers = new WorkerInstance[n_workers];
    for (int i = 0; i < n_workers; i++) {
        workers[i].manager = this;
    }

    auto len = strlen(path);
    auto *item = new char[len + 1];
    strcpy(item, path);
    item[len] = 0;
    directories.push(item);
}

void Manager::set_matcher(Matcher *matcher) {
    for (int i = 0; i < n_workers; i++) {
        workers[i].send_set_matcher(matcher);
    }
}

void Manager::add_directory(char *path) {
    directories.push(path);
    n_dirs++;
    
    char buf[4096];
    int x = sprintf(buf, "f %d %s\n", n_dirs, path);
    write(0, buf, x);
}

void Manager::add_file(char *path) {
    results.push(path);
    char buf[4096];
    int x = sprintf(buf, "f %d %s\n", results.size(), path);
    write(0, buf, x);
}

void Manager::run() {
    while (1) {
        // Distribute tasks to workers evenly
        for (int i = 0; i < n_workers; i++) {
            // Assign directories
            while (directories.size() > 0 && workers[i].assign_worker(directories.pop()));
            workers[i].read_messages();
            workers[i].write_messages();
        }

        // Look for idle
        bool all_idle = true;
        for (int i = 0; i < n_workers; i++) {
            if (workers[i].n_processing > 0) {
                all_idle = false;
                break;
            }
        }

        // Are we done with all our tasks?
        if (directories.size() == 0 && all_idle) {
            //return;
        }
    }
}


#endif // ___MANAGER_HPP___
