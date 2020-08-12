#if !defined(___MANAGER_HPP___)
#define ___MANAGER_HPP___

#include <poll.h>
#include <sys/select.h>

#include "./util.hpp"
#include "./stack.hpp"
#include "./worker.hpp"

enum WorkerInstanceStatus {
    wis_idle, wis_active, wis_halting, wis_dead
};

class WorkerInstance;

class Manager {
private:
    WorkerInstance *workers;
    int n_workers;
    Stack<WorkerInstance*> available;

    LinkedList<char*> *results = nullptr;
    LinkedList<char*> *directories = nullptr;
public:
    Manager(int n_workers, char* path);
    void set_matcher(Matcher *matcher);
    void add_directory(char *path);
    void add_file(char *path);
    void run();
};

class WorkerInstance {
    WorkerInstanceStatus status;
    int i;
    void receive_message();
    void receive_change_state();
    void receive_directory();
    void receive_file();
public:
    int mowi, miwo, child_pid, n_processing;
    Manager *manager;
    WorkerInstance();

    void send_set_matcher(Matcher *matcher);
    void terminate_worker();
    void assign_worker(char *path);
    void read_messages(pollfd *pollfd);

    ~WorkerInstance();
};

void WorkerInstance::receive_message() {
    WorkerMessageType type;
    read(miwo, &type, sizeof(type));
    switch (type) {
        case found_file:
            receive_file();
            break;
        case found_directory:
            receive_directory();
            break;
        case change_state:
            receive_change_state();
            break;
    }
}

void WorkerInstance::receive_directory() {
    int size;
    read(miwo, &size, sizeof(int));
    char *path = new char[size + 1];
    read(miwo, path, size);
    path[size] = 0;
    manager->add_directory(path);
}

void WorkerInstance::receive_file() {
    int size;
    read(miwo, &size, sizeof(int));
    char *path = new char[size + 1];
    read(miwo, path, size);
    path[size] = 0;
    manager->add_file(path);
}

void WorkerInstance::receive_change_state() {
    WorkerStatus status;
    read(miwo, &status, sizeof(status));
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

void WorkerInstance::assign_worker(char *path) {
    auto type = scan_path;
    write(mowi, &type, sizeof(ManagerMessageType));
    int size = strlen(path);
    write(mowi, &size, sizeof(int));
    write(mowi, path, size);
}

void WorkerInstance::read_messages(pollfd *pollfd) {
    while (1) {
        receive_message();

        pollfd->fd = miwo;
        pollfd->events = POLLIN;
        poll(pollfd, 1, 0);

        if (!pollfd->revents & POLLIN) break;
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
    directories = new LinkedList<char*>;
    directories->value = new char[strlen(path) + 1];
    strcpy(directories->value, path);
}

void Manager::set_matcher(Matcher *matcher) {
    for (int i = 0; i < n_workers; i++) {
        workers[i].send_set_matcher(matcher);
    }
}

void Manager::add_directory(char *path) {
    auto node = new LinkedList<char*>;
    node->value = path;
    directories = append(directories, node);
    
    char buf[1024];
    int x = sprintf(buf, "%s\n", path);
    write(0, buf, x);
}

void Manager::add_file(char *path) {
    auto node = new LinkedList<char*>;
    node->value = path;
    results = append(results, node);
    char buf[1024];
    int x = sprintf(buf, "%s\n", path);
    write(0, buf, x);
}

void Manager::run() {
    pollfd *pollfds = new pollfd[n_workers];
    for (int i = 0; i < n_workers; i++) {
        pollfds[i].fd = workers[i].miwo;
        pollfds[i].events = POLLIN;
    }

    while (1) {
        // Distribute tasks to workers evenly
        int i = 0;
        while (directories != nullptr) {
            workers[i].assign_worker(directories->value);
            delete directories->value;
            auto next = directories->next;
            delete directories;
            directories = next;

            i = (i + 1) % n_workers;
        }

        // Poll, read inputs when they come
        int n_events = poll(pollfds, n_workers, -1);
        write(0, "a", 1);
        bool all_idle;
        for (int i = 0; i < n_workers; i++) {
            if (pollfds[i].revents & POLLIN) {
                workers[i].read_messages(pollfds + 1);
            }
            all_idle = false;
        }

        // Are we done with all our tasks?
        if (directories == nullptr && all_idle) {
            return;
        }
    }

    delete pollfds;
}


#endif // ___MANAGER_HPP___
