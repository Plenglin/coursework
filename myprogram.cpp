#include <stdio.h>

#define MATRIX_PRINT_WOLFRAM_ALPHA // debugging

#include "matrix.hpp"


int main(int argc, char *argv[]) {
    mp_initialize(0, 1);
    mp_seed_random();

    Matrix<2, 2> a, b;
    mp_randomize(a);
    mp_randomize(b);

    auto c = a * b;

    std::cout << a << "*" << b << "=" << c << std::endl;
}