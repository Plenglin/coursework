#include <iostream>

#define MATRIX_PRINT_WOLFRAM_ALPHA // debugging

#include "sync.hpp"
#include "matrix.hpp"


typedef Matrix<2, 2> Mat;

struct SharedData {
    Mat a; 
    Mat b;
    Mat c;    
};

int main(int argc, char *argv[]) {
    std::cout << argc << std::endl;
    /*SharedData *shared;
    MPResourceManager<SharedData> mp_mgr("astrid_yu_final_shm", shared, 0, 1);
    
    mp_seed_random();

    Matrix<2, 2> a, b;
    mp_randomize(a);
    mp_randomize(b);

    Matrix<2, 2> c;
    auto pc = &c;
    pc <<= a * b;

    std::cout << a << "*" << b << "=" << c << std::endl;*/
}