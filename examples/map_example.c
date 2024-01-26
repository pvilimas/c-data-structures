#include "../map.h"

#include <stdio.h>

typedef struct {
	const char* s;
	int x;
} MyStruct;

int main() {

	// this is a valid declaration:
	map(MyStruct) a = map_new(MyStruct);

	// but these are not:
	// MyStruct* b = map_new(MyStruct);
	// void* c = map_new(MyStruct);

	// map_insert and map_get return the associated value:
	printf("value inserted: %d\n", map_insert(a, "hi",
		(MyStruct){ .s = "hi" }).x);
	printf("value: %d\n", map_get(a, "hi").x);

	// you can also check if elements are in the map:

	// you can remove elements
	printf("the key '%s' %s in the map\n", "hi",
		map_contains(a, "hi") ? "is" : "is not");
		
	map_remove(a, "hi");

	printf("the key '%s' %s in the map\n", "hi",
		map_contains(a, "hi") ? "is" : "is not");

	const char* str = "abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < 26; i++) {
		map_insert(a, str + i, (MyStruct) { .s = str + i, .x = i });
	}

	// how to iterate over all keys in the map (order is random):

	// this function must be called first:
	map_iter_start(a);
	// (since there is no separate iterator type, only one iteration can be
	// in progress at once for each map)
	while (map_iter_has_next(a)) {
		// don't call next_key without calling has_next first
		const char* key = map_iter_next_key(a);
		// the iterator stuff only gives you keys, but you can easily get values:
		MyStruct value = map_get(a, key);
		printf("got key '%s' and value %d\n", key, value.x);
	}
	// map_iter_next_key will safely return NULL if there are no more keys

	map_free(a);
	// map_free(b);

	return 0;
}
