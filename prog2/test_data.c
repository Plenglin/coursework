#include "data.h"
#include "stdio.h"
#include <assert.h>


void test_chunksize_fits() {
    Chunk chunk;
    assert((char*)&chunk.leader - (char*)&chunk <= 128);
}

void test_insert_manager_reordering() {
    Chunk chunks[12] = {0};
    Chunk *chunk = (Chunk*)&chunks;
    Manager *manager = &chunks[0].manager;

    assert(manager->employees.count == 0);
   
    insert_at(chunk, manager, 0, 1, chunks + 3);
    assert(manager->employees.count == 1);
    assert(manager->employees.sizes.a[0] == 1);
    assert(manager->employees.locations.a[0] == &chunks[3]);

    insert_at(chunk, manager, 1, 2, chunks + 4);
    assert(manager->employees.count == 2);
    assert(manager->employees.sizes.a[0] == 1);
    assert(manager->employees.sizes.a[1] == 2);
    assert(manager->employees.locations.a[0] == &chunks[3]);
    assert(manager->employees.locations.a[1] == &chunks[4]);

    insert_at(chunk, manager, 0, 3, chunks + 1);
    assert(manager->employees.count == 3);
    assert(manager->employees.sizes.a[0] == 3);
    assert(manager->employees.sizes.a[1] == 1);
    assert(manager->employees.sizes.a[2] == 2);
    assert(manager->employees.locations.a[0] == &chunks[1]);
    assert(manager->employees.locations.a[1] == &chunks[3]);
    assert(manager->employees.locations.a[2] == &chunks[4]);

    insert_at(chunk, manager, 2, 4, chunks + 2);
    assert(manager->employees.count == 4);
    assert(manager->employees.sizes.a[0] == 3);
    assert(manager->employees.sizes.a[1] == 1);
    assert(manager->employees.sizes.a[2] == 4);
    assert(manager->employees.sizes.a[3] == 2);
    assert(manager->employees.locations.a[0] == &chunks[1]);
    assert(manager->employees.locations.a[1] == &chunks[3]);
    assert(manager->employees.locations.a[2] == &chunks[2]);
    assert(manager->employees.locations.a[3] == &chunks[4]);
}

void test_insert_manager_overflow() {
    Chunk chunks[30];
    Chunk *chunk = (Chunk*)&chunks;
    Manager *manager = &chunks[0].manager;
    
    insert_at(chunk, manager, 0, 1, chunks + 2);
    insert_at(chunk, manager, 1, 2, chunks + 3);
    insert_at(chunk, manager, 2, 3, chunks + 4);
    insert_at(chunk, manager, 3, 4, chunks + 5);
    assert(manager->employees.count == 4);
    
    insert_at(chunk, manager, 0, 5, chunks + 1);
    assert(manager->employees.count == 4);
    assert(manager->employees.sizes.a[0] == 5);
    assert(manager->employees.sizes.a[1] == 1);
    assert(manager->employees.sizes.a[2] == 2);
    assert(manager->employees.sizes.a[3] == 3);
    
    Chunk *next = chunks + 5;
    Manager *next_manager = &next->manager;
    assert(manager->next == next);
    assert(manager->employees.count == 4);
    assert(next_manager->employees.count == 0);
}

int main() {
    test_insert_manager_reordering();
    test_insert_manager_overflow();
}
