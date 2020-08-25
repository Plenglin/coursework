#include <iostream>

//#define MATRIX_PRINT_NUMPY  // debugging/verification


#include "sync.hpp"
#include "matrix.hpp"


#define MAT_SIZE 32
typedef Matrix<MAT_SIZE, MAT_SIZE> Mat;

struct SharedData {
    Mat a; 
    Mat b;
    Mat c;    
};

int main(int argc, char *argv[]) {
    SharedData *shr;

    // Note that this automatically cleans up everything
    // when it goes out of scope.
    MPResourceManager<SharedData> mp_mgr(
        "astrid_yu_final_shm", 
        shr, 
        atoi(argv[1]), 
        atoi(argv[2])); 
    mp_seed_random();

    mp_randomize(shr->a);
    mp_randomize(shr->b);
    mp_synch();

    clock_t start;
    LEADER {
        start = clock();
    }

    shr->c <<= shr->a * shr->b;
    mp_synch();
    shr->a <<= shr->b * shr->c;
    mp_synch();
    shr->b <<= shr->a * shr->c;
    mp_synch();

    LEADER {
        clock_t duration = clock() - start;
        std::cout << "A:" << std::endl << shr->a << std::endl << std::endl;
        std::cout << "B:" << std::endl << shr->b << std::endl << std::endl;
        std::cout << "C:" << std::endl << shr->c << std::endl << std::endl;
        std::cout << "Computed in " << duration / (float)CLOCKS_PER_SEC << " seconds" << std::endl;
    }
}