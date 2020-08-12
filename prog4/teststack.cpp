#include "stack.hpp"

void htl(int sig){}

int main() {
    SharedStack<int> stack(2);
    int fds[2];
    char buf[1000];
    pipe(fds);
    int child = fork();
    if (child) {
        stack.set_proc_i(0);
        sleep(1);
        sprintf(buf, "pushing\n");
        write(0, buf, strlen(buf));  
        stack.push(1);
        sleep(1);     
        kill(child, SIGUSR1);      
        //stack.push(1);
        sleep(100);
    } else {
        signal(SIGUSR1, htl);
        stack.set_proc_i(1);
        int x[10];
        int n = stack.pop(3, 10, x);
        sprintf(buf, "got %d %d", n, x[0]);
        write(0, buf, strlen(buf));
        sleep(100);
        exit(0);
    }
    wait(0);
    return 0;
}