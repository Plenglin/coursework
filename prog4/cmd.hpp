#ifndef ___CMD_HPP___
#define ___CMD_HPP___

#include <stdio.h> 

#include "./child.hpp"


void print_prog_status(int status) {
    if (status) {  // Error
        write(STDOUT_FILENO, "\033[0;31m", 8);  // Red
    } else {  // Normal
        write(STDOUT_FILENO, "\033[0;34m", 8);  // Blue
    }
    const_print("superduper finder program \033[0;36m");
    //write(0, path, strlen(path));
    const_print("\033[0m$ ");
    fflush(0);
}

int execute_list() {
    printf("List of procs\n");
    int proc_count = 0;
    for (int i = 0; i < MAX_CHILD_PROCS; i++) {
        auto proc = all_procs + i;
        if (!proc->pid) continue;
        proc_count++;
        printf("%d. child PID: %d, state: ", i, proc->pid);
        switch (*proc->state) {
            case proc_active:
                printf("active\n");
                break;
            case proc_terminating:
                printf("terminating\n");
                break;
            case proc_dead:
                printf("dead\n");
                break;
        }
    }
    if (!proc_count) {
        printf("(none)\n");
    }
}

int execute_kill(int index) {
    auto proc = all_procs + 1;
    if (!proc->pid) {
        printf("No process exists at index %d\n", index);
        return 1;
    }
    if (!proc->state) {
        printf("Process at index %d is already dead\n", index);
        return 1;
    }
    printf("Killing process index %d (pid %d)\n", index, proc->pid);
    *proc->state = proc_terminating;
}

int parse_find_args(ProcessInfo *proc_info, char *arg_str) {
    char *tok = strtok(arg_str, " \n");
    bool valid = false;
    while (tok != NULL) {
        if (tok[0] == '"') {
            tok = strtok(tok + 1, "\"");
            if (!tok) break;
            int len = strlen(tok);
            proc_info->match_contents = strcpy(new char[len + 1], tok);
            tok = strtok(NULL, " \n");
            valid = true;
            continue;
        } 
        
        if (tok[0] == '-') {
            if (tok[1] == 'f' && tok[2] == ':') {
                strcpy(proc_info->matcher.ext, tok + 3);
            } else if (tok[1] == 's') {
                proc_info->is_recursive = true;
            }
        } else {
            valid = true;
            strcpy(proc_info->matcher.name, tok);
        }
        
        tok = strtok(NULL, " \n");
    }
    return valid;
}

enum Command {
    cmd_none = 0, cmd_find, cmd_list, cmd_kill, cmd_echo, cmd_quit
};

char* get_command_type(char *str, Command &command) {
    char *end_cmd = strchr(str, ' ');
    if (end_cmd != nullptr) {
        *end_cmd = '\0';
        end_cmd++;
    } else {
        end_cmd = strchr(str, '\n');
        if (end_cmd != nullptr) {
            *end_cmd = '\0';
        }
    }

    char *tok = str;
    if (!strcmp(tok, "find")) {
        command = cmd_find;
    } else if (!strcmp(tok, "list")) {
        command = cmd_list;
    } else if (!strcmp(tok, "kill")) {
        command = cmd_kill;
    } else if (!strcmp(tok, "echo")) {  // Used by child procs to print results
        command = cmd_echo;
    } else if (!strcmp(tok, "quit") || !strcmp(tok, "q")) {
        command = cmd_quit;
    } else {
        command = cmd_none;
        return nullptr;
    }
    return end_cmd;
}

int execute_command(char *str, ProcessInfo *next_proc) {
    Command command;
    char *args = get_command_type(str, command);

    switch (command) {
        case cmd_echo:
            printf("%s", args);
            return 0;
        case cmd_find:
            if (!next_proc) {
                printf("No more available processes\n");
                return 2;
            }
            if (!args || !parse_find_args(next_proc, args)) { 
                printf("Must provide args to find\n");
                return 3;
            }
            dispatch_proc(next_proc);
            return 0;
        case cmd_list:
            return execute_list();
        case cmd_kill: {
            int i;
            sscanf(args, "%d", &i);
            return execute_kill(i);
        }
        case cmd_quit:
            return -1;
    }
    printf("Invalid command\n");
    return 1;
}

void handle_child_print() {
    auto proc = all_procs + *interrupting_proc_i;
    int *fd = proc->fds;

    char buf[4096];
    int n = read(STDIN_FILENO, buf, 1);
    while (n > 0) {
        write(STDOUT_FILENO, buf, n);
        n = read(STDIN_FILENO, buf, 1);
    }
    dup2(stdin_save, STDIN_FILENO);

    *interrupting_proc_i = -1;
}

void run_cmd_loop_until_quit() {
    char buf[4096];
    int status = 0;
    while (1) {
        print_prog_status(status);

        for (int i = 0; i < 4096; i++) {
            int n = read(STDIN_FILENO, buf + i, 1);
            if (!n || buf[i] == '\n') {
                break;
            }
        }
        if (*interrupting_proc_i != -1) {
            printf("\n");
            handle_child_print();
            continue;
        }

        auto proc_info = get_available_proc();
        status = execute_command(buf, proc_info);
        if (status < 0) {
            printf("Goodbye\n");
            return;
        }
    }

    return;
}


#endif // ___CMD_HPP___