#ifndef ___CMD_HPP___
#define ___CMD_HPP___

#include "./child.hpp"

int parse_find_args(ProcessInfo *proc_info, char *arg_str) {
    char *tok = strtok(arg_str, " \n");
    bool valid = false;
    while (tok != NULL) {
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
                printf("No more available processes");
                return 2;
            }
            if (!args || !parse_find_args(next_proc, args)) { 
                printf("Must provide args to find");
                return 3;
            }
            dispatch_proc(next_proc);
            return 0;
        case cmd_list:
            //todo 
            break;
        case cmd_kill:
            //todo
        case cmd_quit:
            return -1;
    default:
        break;
    }

    return 0;
}

void run_cmd_loop_until_quit() {
    char buf[4096];

    //int stdin_alias = dup(STDIN_FILENO); 
    int status = 0;

    while (1) {
        fgets(buf, 4096, stdin);
        auto proc_info = get_available_proc();
        status = execute_command(buf, proc_info);
        if (status < 0) {
            printf("Goodbye\n");
            return;
        }
        if (status) {
            printf("Invalid arguments");
        }
    }

    return;
}


#endif // ___CMD_HPP___