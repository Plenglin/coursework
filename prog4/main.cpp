#include "./cmd.hpp"

int main() {
    init_procs();
    run_cmd_loop_until_quit();
    destroy_procs();
    return 0;
}
