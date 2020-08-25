#include <iostream>

#define MATRIX_PRINT_WOLFRAM_ALPHA // debugging/verification

#include "sync.hpp"
#include "matrix.hpp"


typedef Matrix<4, 4> Mat;

struct SharedData {
    Mat a; 
    Mat b;
    Mat c;    
};

int main(int argc, char *argv[]) {
    SharedData *shared;
    MPResourceManager<SharedData> mp_mgr(
        "astrid_yu_final_shm", 
        shared, 
        atoi(argv[1]), 
        atoi(argv[2])); 
    mp_seed_random();

    auto A = &shared->a;
    auto B = &shared->b;
    auto C = &shared->c;
    mp_randomize(A);
    mp_randomize(B);
    mp_synch();
    
    C <<= shared->a * shared->b;
    mp_synch();

    LEADER {
        std::cout 
            << shared->a << std::endl 
            << "*" << std::endl
            << shared->b << std::endl 
            << "=" << std::endl 
            << shared->c << std::endl;
    }
}