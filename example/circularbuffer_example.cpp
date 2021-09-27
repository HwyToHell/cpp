#include "../inc/circularbuffer.h"
#include <iostream>

void printBuffer(CircularBuffer<int> cb)
{
    std::cout << "head";
    for (size_t n = 0; n < cb.size(); ++n) {
        std::cout << "+" << n << ":" << cb.at(n) << " ";
    }
    std::cout << std::endl;
}

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    CircularBuffer<int> cb(5);

    std::vector<int> vec(10);
    vec[0] = 10;
    vec[9] = 19;
    std::cout << vec.size() << std::endl;

    std::cout << "--- fill buffer ----------------------------------------------------------------" << std::endl;
    for (int i = 1; i < 7; ++i) {
        cb.push(i);
        std::cout << "push: " << i << " front: " << cb.front() << " size: " << cb.size() << " full: " << cb.isFull() << std::endl;
        printBuffer(cb);
        std::cout << std::endl;
    }

    std::cout << std::endl << "--- discharge buffer -----------------------------------------------------------" << std::endl;
    for (size_t i = 0; i < 3; ++i) {
        cb.pop();
        std::cout << "pop: " << i << " front: " << cb.front() << " size: " << cb.size() << " full: " << cb.isFull() << std::endl;
        printBuffer(cb);
        std::cout << std::endl;
    }

    return 0;
}
