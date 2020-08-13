#include "./child.hpp"

int main() {
    ProcessInfo procs[10];

    ProcessInfo proc;
    proc.path = strcpy(new char[100], ".");
    strcpy(proc.matcher.name, "child");
    proc.matcher.filter = by_name;
    proc.is_recursive = true;

    do_child(&proc);
    return 0;
}
