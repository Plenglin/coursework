#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <dirent.h>
#include <sys/stat.h>
#include <vector>
#include <string>

#include "./util.hpp"
#include <stdio.h>

int main() {
    char p[] = "./.gitignore";
    char m[] = "asdffdf";
    bool foo = file_contains_contents(p, m);

    printf("%d", foo);
}