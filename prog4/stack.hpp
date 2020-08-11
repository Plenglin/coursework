#if !defined(___STACK_HPP___)
#define ___STACK_HPP___

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <sys/stat.h>

#include "util.hpp"

template <class T>
class Stack
{
private:
    T *data;
    int capacity;
    int n = 0;

    void resize(int new_capacity) {
        T *new_data = malloc_shared<T>(new_capacity);
        for (int i = 0; i < n; i++) {
            new_data[i] = data[i];
        }
        free_shared(data, capacity);

        data = new_data;
        capacity = new_capacity;
    }
public:
    Stack(int capacity = 1000) : capacity(capacity) {
        data = malloc_shared<T>(capacity);
    }

    int size() {
        return n;
    }

    void push(T obj) {
        if (n >= capacity) {
            resize(capacity * 2);
        }
        data[n++] = obj;
    }

    T pop() {
        return data[--n];
    }

    void free() {
        free_shared(data, capacity);
    }
};

/**
 * A stack that multiple processes can use together. Used as the central task queue. 
 * Note that I'm using queue and stack somewhat interchangeably, because we want to do a 
 * parallelized DFS rather than a BFS so that there isn't ridiculous memory usage.
 */
template <class T>
class SharedStack {
private:
    Stack<T> *stack;
    Mutex stack_mutex;
    int proc_i;
    
    /**
     * Write pipes being listened to by processes waiting on a new item.
     */
    Stack<int> *waiting_procs;
    Mutex waiting_procs_mutex;

    int *pipes;
    int n_procs;
public:
    SharedStack(int n_procs) : n_procs(n_procs), stack_mutex(n_procs), waiting_procs_mutex(n_procs) {
        stack = malloc_shared<Stack<T>>(1);
        *stack = Stack<T>();
        waiting_procs = malloc_shared<Stack<int>>(1);
        *waiting_procs = Stack<int>();
        pipes = malloc_shared<int>(n_procs * 2);
        for (int i = 0; i < n_procs; i++) {
            pipe(&pipes[i * 2]);
        }
    }

    void set_proc_i(int proc_i) {
        this->proc_i = proc_i;
        stack_mutex.set_i(proc_i);
        waiting_procs_mutex.set_i(proc_i);
    }

    int size() {
        auto lock = stack_mutex.lock();
        return stack->size();
    }

    void push(T obj) {
        {
            // Acquire stack ownership and push
            auto lock = stack_mutex.lock();
            stack->push(obj);
        }
        {
            // Notify waiting process that there is another task, if there are waiting processes
            auto lock = waiting_procs_mutex.lock();
            if (waiting_procs->size() > 0) {
                int fd = waiting_procs->pop();
                char x = 0;
                write(fd, &x, 1);
            }
        }
    }

    /**
     * Attempts to pop multiple items from the stack. If no items exist, waits until 
     * there are items. Returns the number of items popped.
     */
    int pop(int min_n, int max_n, T *out) {
        int n = 0;
        while (n < max_n) {
            {
                // Pop if there are items
                auto lock = stack_mutex.lock();
                if (stack->size() > 0) {
                    out[n++] = stack->pop();
                    continue;
                }
            }

            // If we've popped enough to satisfy, then leave
            if (n >= min_n) {
                break;
            }

            // We have not popped enough items to satisfy the lord. Wait for more.
            {
                // Submit pipe to waiting_procs
                auto lock = waiting_procs_mutex.lock();
                waiting_procs->push(pipes[proc_i * 2 + 1]);
            }

            // Read pipe (aka wait for pipe to be closed)
            char buf[100];
            read(pipes[proc_i * 2], buf, 1);
        }
        return n;
    }

    ~SharedStack() {
        close(pipes[proc_i * 2 + 1]);
        if (proc_i != 0) return;
        stack->free();
        waiting_procs->free();
        free_shared(stack, 1);
        free_shared(waiting_procs, 1);
    }
};


#endif // ___STACK_HPP___
