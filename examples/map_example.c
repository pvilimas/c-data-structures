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
		(MyStruct){ .x = 9 }).x);
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
	// nested iteration will not work
	// do not modify the map during iteration

	// call has_next to iterate to the next key-value pair
	while(map_has_next(a)) {
		// then use these functions to get the current key or value
		const char* key = map_key(a);
		MyStruct value = map_value(a);
		
		// do something with key and value
		if (value.x == 5) {
			// always call this function when breaking from a loop
			map_iter_stop(a);
			break;
		}
	}

	map_free(a);

	return 0;
}
