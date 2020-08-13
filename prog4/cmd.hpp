#ifndef ___CMD_HPP___
#define ___CMD_HPP___

#include "./child.hpp"

void parse_args(ProcessInfo *proc_info, char *arg_str) {
    char *tok = strtok(arg_str, " ");
    while (tok != NULL) {
        if (tok[0] == '-') {
            if (tok[1] == 'f' && tok[2] == ':') {
                strcpy(proc_info->matcher.ext, tok + 3);
            } else if (tok[1] == 's') {
                proc_info->is_recursive = true;
            }
        } else {
            strcpy(proc_info->matcher.name, tok);
        }
        
        tok = strtok(NULL, " ");
    }
}


#endif // ___CMD_HPP___