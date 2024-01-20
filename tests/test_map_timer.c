#include "map.h"

#include <stdio.h>
#include <assert.h>
#include <time.h>

clock_t start_time;

void start_timer() {
    start_time = clock();
}

double stop_timer() {
    clock_t end_time = clock();
    return ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
}

int main() {
    double time_elapsed;

    // Step 1: Create a map
    start_timer();
    map myIntMap = map_new();
    time_elapsed = stop_timer();
    printf("Map creation time: %f seconds\n", time_elapsed);

    // Step 2: Add 1 million entries to the map
    start_timer();
    for (int i = 0; i < 1000000; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        int value = i * 2;
        map_set(&myIntMap, key, value);
    }
    time_elapsed = stop_timer();
    printf("Insertion time: %f seconds\n", time_elapsed);

    // Step 3: Verify that each key is still there and has the correct value
    start_timer();
    for (int i = 0; i < 1000000; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        assert(map_contains(&myIntMap, key));
        assert(*map_get(&myIntMap, key) == i * 2);
    }
    time_elapsed = stop_timer();
    printf("Lookup time: %f seconds\n", time_elapsed);

	// Step 3.5: Copy all keys to another map
    start_timer();
    map myIntMap2 = map_copy(&myIntMap);
    time_elapsed = stop_timer();
    printf("Copy time: %f seconds\n", time_elapsed);

    // Step 3.6: Verify that all keys are in the new map 
    for (int i = 0; i < 1000000; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        assert(map_contains(&myIntMap2, key));
    }

    // Step 4: Delete keys ending in an odd number
    start_timer();
    for (int i = 1; i < 1000000; i += 2) {
        char key[20];
        sprintf(key, "key%d", i);
        map_delete(&myIntMap, key);
    }
    time_elapsed = stop_timer();
    printf("Deletion time: %f seconds\n", time_elapsed);

	map_free(&myIntMap);

    return 0;
}

