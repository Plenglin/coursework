#pragma once

#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <string>


int __sync_i, __sync_n;
int *__ready_arr;

#define LEADER_I 0
#define LEADER if (__sync_i == LEADER_I)
#define FOLLOWER if (__sync_i != LEADER_I)

void mp_synch() {
    int synchid = __ready_arr[__sync_n] + 1; 
    __ready_arr[__sync_i] = synchid; 
    while (true) {
        bool breakout = true; 
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
    int fd = 0;
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
        LEADER {
            // Leader creates shared memory and truncates.
            shm_unlink(shm_name.c_str());  // Unlink if exists

            fd = shm_open(shm_name.c_str(), O_RDWR | O_CREAT, 0777);
            ftruncate(fd, alloc_size);

            std::cout << i << "/" << __sync_n << " Creating shared memory " << shm_name << " at fd=" << fd << std::endl;
        } else {
            // Wait for leader to open shared memory.
            while (fd <= 0) {
                usleep(50000);
                fd = shm_open(shm_name.c_str(), O_RDWR, 0777);
            }
            std::cout << i << "/" << __sync_n << " opened " << shm_name << " at fd=" << fd << std::endl;
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
            // Leader cleans ready array.
            for (int j = 0; j < __sync_n + 1; j++) {
                __ready_arr[j] = 0;
            }
            for (int j = 0; j < __sync_n + 1; j++) {
                std::cout << __sync_i << ": " << __ready_arr[j] << "\n";
            }
        } else {
            // Follower waits for ready array to have been fully cleaned.
            while (true) {
                bool breakout = true;
                for (int j = 0; j < __sync_n + 1; j++) {
                    if (j != LEADER_I && __ready_arr[j] != 0) {
                        breakout = false;
                        break;
                    }
                }
                if (breakout) {
                    break;
                }
            }
            for (int j = 0; j < __sync_n + 1; j++) {
                std::cout << __sync_i << ": " << __ready_arr[j] << "\n";
            }
        }

        std::cout << i << "/" << __sync_n << " ready" << std::endl;
        mp_synch();
    }

    ~MPResourceManager() {
        mp_synch();
        close(fd);
        munmap(mmap_base, alloc_size);
        LEADER {
            std::cout << "Unlinking shared memory" << std::endl;
            shm_unlink(shm_name.c_str());
        }
    }
};

void mp_seed_random() {
    srand(time(NULL) * (__sync_n + __sync_i) / __sync_n);
}

