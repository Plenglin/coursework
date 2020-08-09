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
    int *capacity;
    int *n;

    void resize(int new_capacity) {
        T *new_data = malloc_shared<T>(new_capacity);
        for (int i = 0; i < *n; i++) {
            new_data[i] = data[i];
        }
        free_shared(data, *capacity);

        data = new_data;
        *capacity = new_capacity;
    }
public:
    Stack(int users, int capacity = 1000) {
        n = malloc_shared<int>(2);
        *n = 0;
        this->capacity = n + 1;
        *this->capacity = capacity;
        data = malloc_shared<T>(capacity);
    }

    int size() {
        return *n;
    }

    void push(T obj) {
        if (*n >= *capacity) {
            resize(*capacity * 2);
        }
        data[(*n)++] = obj;
    }

    T pop() {
        if (n <= 0) {
            return;
        }
        return data[--(*n)];
    }

    ~Stack() {
        free_shared(n, 2);
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
    Stack<T> stack;
    Mutex stack_mutex;
    
    /**
     * Write pipes being listened to by processes waiting on a new item.
     */
    Stack<int> waiting_procs;
    Mutex waiting_procs_mutex;
public:
    SharedStack(int n_procs) : stack_mutex(n_procs), waiting_procs_mutex(n_procs) {
        stack = malloc_shared<Stack<T>>();
    }

    void set_proc_i(int proc_i) {
        stack_mutex.set_i(proc_i);
        waiting_procs_mutex.set_i(proc_i);
    }

    int size() {
        auto lock = stack_mutex.lock();
        return stack->size();
    }

    void push(T obj) {
        // Acquire stack ownership and push
        auto l1 = stack_mutex.lock();
        stack->push(obj);
        {
            // Notify waiting process that there is another task, if there are waiting processes
            auto l2 = waiting_procs_mutex.lock();
            if (waiting_procs.size() > 0) {
                int fd = waiting_procs.pop();
                close(fd);
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
                if (stack.size() > 0) {
                    out[n++] = stack.pop();
                    continue;
                }
            }

            // If we've popped enough to satisfy, then leave
            if (n >= min_n) {
                break;
            }

            // We have not popped enough items to satisfy the lord. Wait for more.
            int fd[2];
            pipe(fd);
            {
                // Submit pipe to waiting_procs
                auto lock = waiting_procs_mutex.lock();
                waiting_procs.push(fd[1]);
            }

            // Read pipe (aka wait for pipe to be closed)
            char buf;
            read(fd[0], buf, 1);
        }
        return n;
    }

    ~SharedStack() {

    }
};


#endif // ___STACK_HPP___
