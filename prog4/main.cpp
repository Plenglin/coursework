#include "./child.hpp"

int main() {
    ProcessInfo procs[10];
    char path[] = "/";
    Matcher matcher;
    scan_path_recursive(&matcher, path);
    return 0;
}
