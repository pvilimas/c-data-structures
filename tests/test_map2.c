#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// test map_iter

int main() {

    int size;

    map(int) m = map_new(int, NULL);
    map_insert(m, "a", 5);
    map_insert(m, "ab", 6);
    map_insert(m, "abc", 7);
    map_insert(m, "abcd", 8);
    map_insert(m, "abcef", 9);

    while(map_has_next(m)) {
		const char* key = map_key(m);
		int* value = map_value(m);
        printf("%s : %d\n", key, *value);
	}

    while(map_has_next(m)) {
		const char* key = map_key(m);
		int* value = map_value(m);
        printf("%s : %d\n", key, *value);
	}

    for (int i = 0; i < 100; i++) {
        size = 0;
        while(map_has_next(m)) {
            const char* key = map_key(m);
            int* value = map_value(m);
            size++;
            if (size == 3) {
                map_iter_stop(m);
                break;
            }
        }
        printf("%d\n", size);
    }

    return 0;
}