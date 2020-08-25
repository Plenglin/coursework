#pragma once

#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>



int __sync_i, __sync_n;
int *__ready_arr;
#define LEADER if (__sync_i == 0)
#define FOLLOWER if (__sync_i != 0)

void mp_synch() {
    int synchid = __ready_arr[__sync_n] + 1; 
    __ready_arr[__sync_i] = synchid; 
    bool breakout = false;
    while (true) {
        breakout = true; 
        for (int i = 0; i < __sync_n; i++) {
            if (__ready_arr[i] < synchid) {
                breakout = false;
                break;
            }
        }
        if (breakout) {
            __ready_arr[__sync_n] = synchid;
            break;
        }
    }
}

/**
 * RAII object for managing the lifetime of the synch/shared objects.
 */
template <class Data>
class MPResourceManager {
    std::string shm_name;
    size_t ready_arr_size, alloc_size;
    void *mmap_base;
public:
    MPResourceManager(
        std::string shm_name,
        Data* &data_ptr,
        int i, 
        int n
    ) : 
        shm_name(shm_name)
    {
        // Initialize sync numbers
        __sync_i = i;
        __sync_n = n;

        // Initialize sync array
        ready_arr_size = sizeof(int) * (n + 1);
        alloc_size = ready_arr_size + sizeof(Data);

        // Loading shared memory phase.
        int fd = 0;
        LEADER {
            // Create shared memory and truncate.
            fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT, 0777);
            ftruncate(fd, alloc_size);
        } else {
            // Wait for leader to open shared memory.
            while (!fd) {
                fd = shm_open(shm_name.c_str(), O_RDWR, 0777);
                usleep(1000);
            }
        }

        // Memory map to the data array
        mmap_base = mmap(
            NULL, 
            alloc_size, 
            PROT_READ | PROT_WRITE,
            MAP_SHARED,
            fd,
            0);
        
        // Initialize important pointers.
        __ready_arr = (int*)mmap_base;
        data_ptr = (Data*)((char*)mmap_base + ready_arr_size);
        
        LEADER {
            // Clean ready array.
            for (int i = 0; i < __sync_n + 1; i++) {
                __ready_arr[i] = 0;
            }
        } else {
            // Wait for ready array to have been fully cleaned.
            while (true) {
                bool breakout = true;
                for (int j = 0; j < n + 1; j++) {
                    if (__ready_arr[j] != 0) {
                        breakout = false;
                    }
                }
                if (breakout) {
                    break;
                }
            }
        }
    }

    ~MPResourceManager() {
        mp_synch();
        munmap(mmap_base, alloc_size);
        LEADER {
            shm_unlink(shm_name.c_str());
        }
    }
};

void mp_seed_random() {
    srand(time(NULL) * (__sync_n + __sync_i) / __sync_n);
}

