#include <sys/types.h> 
#include <sys/wait.h> 
#include <unistd.h> 
#include <string.h>

#include <string>
#include <iostream>

int n_procs;
int *pids;

void isr(int sig) {
    for (int i = 0; i < n_procs; i++) {
        kill(pids[i], SIGTERM);
    }
}

int main(int argc, char *argv[]) {
    char* prog = argv[1];
    n_procs = atoi(argv[2]);
    std::string n_procs_str = std::to_string(n_procs);

    std::cout << "Astrid MPI initializing " << n_procs_str << " instances of " << prog << std::endl;   
    pids = new int[n_procs];
    char * args[] = {prog, NULL, (char*)n_procs_str.c_str(), NULL};
    for (int i = 0; i < n_procs; i++) {
        int pid = fork();
        if (pid) {
            pids[i] = pid;
            continue;
        }
        auto i_str = std::to_string(i);
        args[1] = (char*)i_str.c_str();
        execv(prog, args);
    }

    signal(SIGINT, isr);
    wait(0);

    delete pids;
    return 0;
}