#include "./child.hpp"

int main() {
    ProcessInfo procs[10];
    char path[] = ".";
    Matcher matcher;
    matcher.filter = by_name | by_ext;
    strcpy(matcher.name, "lab");
    strcpy(matcher.ext, "zip");
    std::vector<char*> results;
    scan_path_recursive(&matcher, path, results);

    for (auto iter = results.begin(); iter != results.end(); iter++) {
        printf("%s\n", *iter);
    }
    return 0;
}
