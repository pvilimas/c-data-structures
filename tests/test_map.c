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
#define D \
	C C C C C C C

const char* str = C C C C C;

#define SIZE strlen(str)

int main() {

	printf("strlen = %zu\n", strlen(str));
	printf("size = %zu\n", (size_t)SIZE);

    map(MyStruct) m = map_new(MyStruct);

	// while(map_has_next(m)) {
	// 	const char* key = map_key(m);
	// 	MyStruct value = map_value(m);
	// }

	map_insert(m, "", (MyStruct){ .x = "empty key", .y = 1 });
	printf("%s\n", map_find(m, "")->x);

	map_remove(m, "abc");

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
	
	size_t size;

	for (int i = 0; i < 100; i++) {
		size = 0;
		while(map_has_next(m)) {
			const char* key = map_key(m);
			MyStruct* value = map_value(m);
			// printf("str<len=%zu> : %d\n", strlen(key), value.y);
			size++;
		}
		assert(size == map_size(m));
	}

    map_free(m);

    printf("all done\n");

    return 0;
}
