#include <iostream>

#define MATRIX_PRINT_NUMPY  // debugging/verification


#include "sync.hpp"
#include "matrix.hpp"


typedef Matrix<32, 32> Mat;

struct SharedData {
    Mat a; 
    Mat b;
    Mat c;    
};

int main(int argc, char *argv[]) {
    SharedData *shr;
    MPResourceManager<SharedData> mp_mgr(
        "astrid_yu_final_shm", 
        shr, 
        atoi(argv[1]), 
        atoi(argv[2])); 
    mp_seed_random();

    mp_randomize(shr->a);
    mp_randomize(shr->b);
    mp_synch();
    
    shr->c <<= shr->a * shr->b;
    mp_synch();
    shr->c <<= shr->a * shr->b;
    mp_synch();
    shr->c <<= shr->a * shr->b;
    mp_synch();

    LEADER {
        std::cout 
            << "np.array_equal(np.matmul(" << shr->a
            << ","
            << shr->b
            << ")," 
            << shr->c << ")" << std::endl;
    }
}