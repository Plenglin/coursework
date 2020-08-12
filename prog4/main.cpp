#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#include "./manager.hpp"

#define STATUS_ALIVE 0
#define STATUS_UNKNOWN 1
#define STATUS_QUIT 2


#define MAX_CHILD_PROCS 10

int main() {
    char path[] = "/";
    Matcher matcher;
    strcpy(matcher.ext, "");
    strcpy(matcher.name, "");
    Manager manager(1, path);
    manager.set_matcher(&matcher);
    manager.run();
    return 0;
}
