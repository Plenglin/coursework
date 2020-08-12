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
class Stack {
private:
    T *data;
    int capacity;
    int n = 0;

    void resize(int new_capacity) {
        T *new_data = new T[new_capacity];
        for (int i = 0; i < n; i++) {
            new_data[i] = data[i];
        }
        delete data;

        data = new_data;
        capacity = new_capacity;
    }
public:
    Stack(int capacity = 1000) : capacity(capacity) {
        data = new T[capacity];
    }

    T* start() {
        return data;
    }

    T* end() {
        return data + n;
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

    ~Stack() {
        delete data;
    }
};


template <class T>
class Queue {
private:
    T *data;
    int capacity;
    int n = 0;
    int start = 0;
    int end = 0;

    void resize(int new_capacity) {
        T *new_data = new T[new_capacity];
        auto iter = start;
        for (int i = 0; i < n; i++) {
            new_data[i] = data[iter];
            iter = (iter + 1) % capacity;
        }
        delete data;

        data = new_data;
        capacity = new_capacity;
        start = 0;
        end = n;
    }
public:
    Queue(int capacity = 1000) : capacity(capacity) {
        data = new T[capacity];
    }

    int size() {
        return n;
    }

    void push(T obj) {
        if (n >= capacity) {
            resize(capacity * 2);
        }
        data[end] = obj;
        end = (end + 1) % capacity;
        n++;
    }

    T pop() {
        int i_out = start;
        n--;
        start = (start + 1) % capacity;
        return data[i_out];
    }

    ~Queue() {
        delete data;
    }
};


#endif // ___STACK_HPP___
