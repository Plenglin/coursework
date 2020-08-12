#include "stack.hpp"

int main() {
    Queue<int> queue(2);
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    int x;
    x = queue.pop();
    x = queue.pop();
    x = queue.pop();
    x = queue.pop();
    queue.push(1);
    queue.push(2);
    queue.push(3);
    queue.push(4);
    x = queue.pop();
    x = queue.pop();
    x = queue.pop();
    x = queue.pop();
    wait(0);
    return 0;
}