#if !defined(___WORKER_HPP___)
#define ___WORKER_HPP___

#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "util.hpp"


enum WorkerMessageType {
    found_file, found_directory, change_state, failed_path
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
public:
    Worker(int mowi, int miwo) : mowi(mowi), miwo(miwo) {

    }

    void run() {
        send_change_state(idle);
        
        while (1) {
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
                    receive_send_path();
                    break;
            }
        }
    }

    void do_terminate() {
        send_change_state(dead);
        close(miwo);
    }

    void receive_send_path() {
        int size;
        read(mowi, &size, sizeof(int));
        char *path = new char[size + 1];
        read(mowi, path, size);
        path[size] = 0;
        
        send_change_state(active);
        process_path(path);
        send_change_state(idle);

        delete path;
    }

    void send_change_state(WorkerStatus status) {
        auto type = change_state;
        write(miwo, &type, sizeof(WorkerMessageType));
        write(miwo, &status, sizeof(WorkerStatus));
    }

    void receive_matcher() {
        read(mowi, &matcher, sizeof(Matcher));
    }

    void process_path(char *path) {        
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
                    auto type = found_directory;
                    write(miwo, &type, sizeof(WorkerMessageType));
                    write(miwo, &len, sizeof(int));
                    write(miwo, buf, len);
                }
                continue;
            }

            // Does the file match the filter?
            if (!matcher.match(dirent->d_name)) continue;

            // Tell the parent about the file match
            int len = build_path(buf, path, dirent->d_name);
            auto type = found_file;
            write(miwo, &type, sizeof(WorkerMessageType));
            write(miwo, &len, sizeof(int));
            write(miwo, buf, len);
        }
        closedir(dir);
    }

    ~Worker() {

    }
};


#endif // ___WORKER_HPP___
