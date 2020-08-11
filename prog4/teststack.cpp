#include "stack.hpp"

int main() {
    SharedStack<int> stack(2);
    int fds[2];
    char buf[1000];
    pipe(fds);
    if (fork()) {
        stack.set_proc_i(0);
        sleep(1);
        sprintf(buf, "pushing\n");
        write(0, buf, strlen(buf));        
        stack.push(1);
        stack.push(1);
        sleep(1);
                sprintf(buf, "pushing\n");
        write(0, buf, strlen(buf));        
stack.push(1);
        sleep(100);
    } else {
        stack.set_proc_i(1);
        int x[10];
        stack.pop(3, 10, x);
        sprintf(buf, "got %d", x);
        write(0, buf, strlen(buf));
        sleep(100);
        exit(0);
    }
    wait(0);
    return 0;
}