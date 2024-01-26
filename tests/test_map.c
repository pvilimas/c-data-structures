#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct {
	const char* x;
	int y;
} MyStruct;

#define A \
	"abcdefghijklmnopqrstuvwxyz"
#define B \
	A A A A A A A A A A A A A A A A A A A A
#define C \
	B B B B B B B

const char* str = C C C C C;

#define SIZE strlen(str)

int main() {

	printf("strlen = %zu\n", strlen(str));
	printf("size = %zu\n", (size_t)SIZE);

    map(MyStruct) m = map_new(MyStruct);

	for (size_t i = 0; i < SIZE; i++) {
		map_insert(m, str + i,
			(MyStruct){ .x = str + i, .y = i + 7 });
	}

	for (size_t i = 0; i < SIZE; i++) {
		assert(map_contains(m, str + i));
		assert(map_get(m, str + i).y == (int)i + 7);
		if (i % 100 > 3) {
			map_remove(m, str + i);
		} else if (i % 100 == 3) {
			map_insert(m, str + i,
				(MyStruct){ .x = str + i, .y = i + 11 });
		}
	}

	for (size_t i = 0; i < SIZE; i++) {
		if (i % 100 > 3) {
			assert(!map_contains(m, str + i));
			assert(map_find(m, str + i) == NULL);
		} else if (i % 100 == 3) {
			assert(map_contains(m, str + i));
			assert(map_find(m, str + i)->y == (int)i + 11);
		} else {
			assert(map_contains(m, str + i));
			assert(map_find(m, str + i)->y == (int)i + 7);
		}
	}
	
	size_t size = 0;
	map_iter_start(m);
	while(map_iter_has_next(m)) {
		const char* c = map_iter_next_key(m);
		if (c) size++;
	}
	assert(size == map_size(m));

    map_free(m);

    printf("all done\n");

    return 0;
}
