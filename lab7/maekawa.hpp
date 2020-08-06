#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>


struct Request {
    int i;
};

struct Inquire {
    int j;
};

struct Yield {
    int k;
};

struct Release {
    int i;
};

struct Grant {
    int j;
};

struct Failed {
    int j;
}

enum MessageType {
    REQUEST, YIELD, INQUIRE, RELEASE, GRANT;
};

struct Message {
    MessageType type;
    union {
        Request request;
        Inquire inquire;
        Yield yield;
        Release release;
        Grant grant;
        Failed failed;
    };
};

class MaekawaLock;

class Maekawa {
private:
    const int n, K;
    int i;
    /**
     * 2D array of pipes[n * to + from], or pipes[from] after set_i is called
     */
    int *read_pipes;
    /**
     * 2D array of pipes[n * from + to], or pipes[to] after set_i is called
     */
    int *write_pipes;
    /**
     * 1D array
     */
    int *quorum_set;

    /**
     * A message buffer way bigger than needed
     */
    Message message_buffer[100];

    int proc_i, reader_proc, timer_proc;
public:
    Maekawa(int n, int K);
    void set_i(int index);
    MaekawaLock lock();
    void on_receive(Message message);
    void do_reader_proc();
    void do_timer_proc();
    void unlock();
    ~Maekawa();
};

class MaekawaLock {
private:
    Maekawa *parent;
public:
    MaekawaLock(Maekawa *parent) : parent(parent) { }
    ~MaekawaLock() {
        parent->unlock();
    }
};

Maekawa::Maekawa(int n, int K) : n(n), K(K) {
    read_pipes = (int*)mmap(
        nullptr,
        sizeof(int) * n * n,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );
    write_pipes = (int*)mmap(
        nullptr,
        sizeof(int) * n * n,
        PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS,
        -1,
        0
    );

    int fds[2];
    for (int fr = 0; fr < n; fr++) {
        for (int to = 0; to < n; to++) {
            pipe(fds);
            int read_fd = fds[0];
            read_pipes[n * to + fr] = read_fd;
            write_pipes[n * fr + to] = fds[1];

            // Set read to be non-blocking
            int flags = fcntl(read_fd, F_GETFL);
            fcntl(read_fd, F_SETFL, flags | O_NONBLOCK);
        }
    }
}

MaekawaLock Maekawa::lock() {
    for (int j = 0; j < K; j++) {
        Message message;
        message.type = REQUEST;
        message.request.i = i;
        write(write_pipes[quorum_set[j]], &message, sizeof(Message));
    }
    while (1) {
        for (int j = 0; j < K; j++) {
            Message message;
            if (read(read_pipes[quorum_set[j]], &message, sizeof(Message)) == EWOULDBLOCK) {
                continue;
            }

            
        }
    }
    return MaekawaLock(this);
}

void Maekawa::set_i(int i) {
    this->i = i;
    read_pipes = read_pipes + i * n;
    write_pipes = write_pipes + i * n;
    quorum_set = new int[K];

    for (int k = 0; k < K; k++) {
        quorum_set[k] = (i + k) % n;
    }
    
    proc_i = getpid();
    reader_proc = fork();
    if (reader_proc == 0) {
        timer_proc = fork();
        if (timer_proc == 0) {

        }
    }
}

void Maekawa::do_reader_proc() {
    
}

void Maekawa::on_receive(Message message) {
    switch (message.type) {
        case REQUEST:
            
            break;
    }
}

Maekawa::~Maekawa() {
    if (i == 0) {
        munmap(read_pipes, sizeof(int) * n * n);
        munmap(write_pipes, sizeof(int) * n * n);
    }
}
