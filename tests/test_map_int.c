#include "../map.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define A \
	"abcdefghijklmnopqrstuvwxyz"
#define B \
	A A A A A A A A A A A A A A A A A A A A
#define C \
	B B B B B
#define D \
	C C C C C
#define E \
	D D D D D D D D D D D D D
#define SIZE len

const char* str = D;

int main() {
	int len = strlen(str);
	printf("strlen = %d\n", len);

	map(int) m = map_new(int);

	for (int i = 0; i < SIZE; i++) {
		map_insert(m, str + i, i + 9);
	}

	for (int i = 0; i < SIZE; i++) {
		assert(map_contains(m, str + i));
		assert(map_get(m, str + i) == i + 9);
	}

	for (int i = 0; i < SIZE; i++) {
		if (i % 10 == 9) {
			map_insert(m, str + i, i + 99);
		} else if (i % 10 == 0 || i % 10 == 5) {
			map_remove(m, str + i);
		}
	}

	for (int i = 0; i < SIZE; i++) {
		if (i % 10 == 9) {
			assert(map_contains(m, str + i));
			assert(map_get(m, str + i) == i + 99);
		} else if (i % 10 == 0 || i % 10 == 5) {
			assert(!map_contains(m, str + i));
		}
	}

	int size = 0;
	map_iter_start(m);
	while(map_iter_has_next(m)) {
		const char* k = map_iter_next_key(m);
		if (k) size++;
	}
	printf("size: %d, map size: %zu\n", size, map_size(m));

	map_free(m);

	printf("done\n");
}
