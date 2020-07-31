#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>


void child_isr(int pid) {
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

int do_stat(char *path) {
    struct stat info;
    if (stat(path, &info)) {
        printf("File not available: %s\n", path);
        return -1;
    }

    // do stat print
    return 0;
}

void print_prog_status(int status, char *path) {
    if (status) {  // Error
        write(0, "\033[0;31m", 8);  // Red
    } else {  // Normal
        write(0, "\033[0;34m", 8);  // Blue
    }
    write(0, "stat prog \033[0;36m", 18);
    write(0, path, strlen(path));
    write(0, "\033[0m$ ", 7);
}

int do_cd(char *path) {
    if (chdir(path)) {
        printf("Error changing into %s\n", path);
        return -1;
    }
    return 0;
}

int process_cmd(char *path, char *input) {
    if (!strcmp(input, "q")) {
        // Quit
        exit(0);
    } else if (!strcmp(input, "list")) {
        // List directory
        return do_list(path);
    } 

    if (input[0] == '/') {
        // CD
        strcat(path, input);
        return do_cd(path);
    }

    strcat(path, "/");
    strcat(path, input);

    if (!strcmp(input, "..")) {
        return do_cd(path);
    } else {
        return do_stat(path);
    }
}

void do_child(int parent_pid) {
    char path[1000];
    char input[1000];

    int status = 0;

    while (1) {
        getcwd(path, 1000);
        print_prog_status(status, path);
        scanf("%s", input);
        status = process_cmd(path, input);
    }
}

