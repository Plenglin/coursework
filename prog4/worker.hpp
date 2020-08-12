#if !defined(___WORKER_HPP___)
#define ___WORKER_HPP___

#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>

#include "stack.hpp"
#include "util.hpp"

#define MAX_TASKS 200

enum WorkerMessageType {
    found_file, found_directory, change_state, failed_path, finished_scan, unprocessed_directory, terminating
};

enum ManagerMessageType {
    set_matcher, scan_path, terminate
};

enum WorkerStatus {
    active, idle, dead
};

class Worker {
private:
    int mowi, miwo;
    Matcher matcher;
    Queue<char*> file_results;
    
    Queue<char*> directory_results;
    Queue<char*> tasks;
public:
    Worker(int mowi, int miwo) : mowi(mowi), miwo(miwo) {

    }

    void read_messages() {
        pollfd pollfd;
        while (1) {
            pollfd.fd = mowi;
            pollfd.events = POLLIN;
            poll(&pollfd, 1, 0);
            if (!(pollfd.revents & POLLIN)) {
                return;
            }

            ManagerMessageType type;
            read(mowi, &type, sizeof(ManagerMessageType));
            switch (type) {
                case ManagerMessageType::set_matcher:
                    receive_matcher();
                    break;
                case ManagerMessageType::terminate:
                    do_terminate();
                    return;
                case ManagerMessageType::scan_path:
                    receive_scan_path();
                    break;
            }
        }
    }

    void run() {        
        while (1) {
            read_messages();

            int i = 0;
            while (tasks.size() > 0) {
                consume_path(tasks.pop());
                i++;
            }
            if (i > 0) {
                auto type = finished_scan;
                write(miwo, &type, sizeof(WorkerMessageType));
                write(miwo, &i, sizeof(int));
            }

            auto type = found_directory;
            while (directory_results.size() > 0) {
                auto str = directory_results.pop();
                write(miwo, &type, sizeof(WorkerMessageType));
                write_sized_str(miwo, str);
                delete str;
            }
        }
    }

    void do_terminate() {
        send_change_state(dead);
        close(miwo);
    }

    void receive_scan_path() {
        char *path = read_sized_str(mowi);
        tasks.push(path);
    }

    void send_change_state(WorkerStatus status) {
        auto type = change_state;
        write(miwo, &type, sizeof(WorkerMessageType));
        write(miwo, &status, sizeof(WorkerStatus));
    }

    void receive_matcher() {
        read(mowi, &matcher, sizeof(Matcher));
    }

    void consume_path(char *path) {        
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
                    directory_results.push(str);
                }
                continue;
            }

            // Does the file match the filter?
            if (!matcher.match(dirent->d_name)) continue;

            // Tell the parent about the file match
            int len = build_path(buf, path, dirent->d_name);
            auto str = strcpy(new char[len + 1], buf);
            str[len] = 0;
            file_results.push(str);
        }
        closedir(dir);
        delete path;
    }

    ~Worker() {

    }
};


#endif // ___WORKER_HPP___
