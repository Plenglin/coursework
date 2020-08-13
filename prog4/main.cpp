#include "./cmd.hpp"

int main() {
    ProcessInfo proc;
    char cmd[] = " child -f:s -s";
    parse_args(&proc, cmd);
    return 0;
}
