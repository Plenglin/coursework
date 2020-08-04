#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>


#define STATUS_ALIVE 0
#define STATUS_UNKNOWN 1
#define STATUS_QUIT 2

#define const_print(str) write(0, str, sizeof(str) - 1)


typedef struct Message {
    int status;
} Message;

int __parent_pid;
Message *__message;

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

int do_stat(char *path) {
    struct stat info;
    if (stat(path, &info)) {
        printf("File not available: %s\n", path);
        return -1;
    }

    printf("stat %s\n", path);
    printf("    dev:                %ld\n", info.st_dev);     /* ID of device containing file */
    printf("    ino:                %ld\n", info.st_ino);     /* inode number */
    printf("    mode:               %d\n", info.st_mode);    /* protection */
    printf("    nlink:              %ld\n", info.st_nlink);   /* number of hard links */
    printf("    uid:                %d\n", info.st_uid);     /* user ID of owner */
    printf("    gid:                %d\n", info.st_gid);     /* group ID of owner */
    printf("    rdev:               %ld\n", info.st_rdev);    /* device ID (if special file) */
    printf("    size:               %ld\n", info.st_size);    /* total size, in bytes */
    printf("    blksize:            %ld\n", info.st_blksize); /* blocksize for file system I/O */
    printf("    blocks:             %ld\n", info.st_blocks);  /* number of 512B blocks allocated */
    printf("    atime:              %ld\n", info.st_atime);   /* time of last access */
    printf("    mtime:              %ld\n", info.st_mtime);   /* time of last modification */
    printf("    ctime:              %ld\n", info.st_ctime);   /* time of last status change */    return 0;
}

void print_prog_status(int status, char *path) {
    if (status) {  // Error
        write(0, "\033[0;31m", 8);  // Red
    } else {  // Normal
        write(0, "\033[0;34m", 8);  // Blue
    }
    const_print("stat prog \033[0;36m");
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

int process_cmd(char *path, char *input) {
    if (!strcmp(input, "q")) {
        // Quit
        const_print("Child quit\n");
        __message->status = STATUS_QUIT;
        exit(0);
    } 
    
    if (!strcmp(input, "list")) {
        // List directory
        return do_list(path);
    } 

    if (input[0] == '/') {
        // CD
        strcat(path, input);
        return do_cd(path);
    }

    if (!strcmp(input, "..")) {
        strcat(path, "/");
        strcat(path, input);
        return do_cd(path);
    } 
    
    return do_stat(input);
}

void do_child(int parent_pid) {
    char path[1000];
    char input[1000];
    __parent_pid = parent_pid;
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

