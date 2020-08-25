#include <stdio.h>
#include "matrix.hpp"


int main(int argc, char *argv[]) {
    mp_initialize(0, 1);

    Matrix<3, 3> m;
    mp_randomize(m);
    std::cout << m << std::endl;
}