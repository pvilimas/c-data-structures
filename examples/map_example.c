#include "../map.h"

#include <stdio.h>

typedef struct {
	const char* s;
	int x;
	void* data;
} MyStruct;

MyStruct MyStruct_new(const char* s, int x) {
	return (MyStruct) {
		.s = s,
		.x = x,
		.data = malloc(100)
	};
}

void MyStruct_free(void* p) {
	MyStruct* m = (MyStruct*)p;
	printf("freeing %d\n", m->x);
	free(m->data);
}

int main() {

	// this is a valid declaration (passing value free fn):
	// value free fn is called when a key is removed, or when the whole
	// map gets freed
	map(MyStruct) a = map_new(MyStruct, MyStruct_free);

	// this is also valid:
	map(int) b = map_new(int, NULL);

	// but these are not:
	// MyStruct* b = map_new(MyStruct);
	// void* c = map_new(MyStruct);

	// map_insert and map_get return the associated value:
	printf("value inserted: %d\n", map_insert(a, "hi",
		MyStruct_new("", 9)).x);
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

	// iteration example

	// this map does not use a separate iterator type, only internal state
	// for controlling the iteration
	
	// this means nested loops will not work
	// also do not modify the map during iteration
	// before using break, always call map_iter_stop to reset everything
	// this is not needed otherwise though, see below

	// call has_next to iterate to the next key-value pair
	while(map_has_next(a)) {
		// then use these functions to get the current key or value
		const char* key = map_key(a);
		MyStruct* value = map_value(a);
		
		// do something with key and value
		if (value->x == 5) {
			// always call this function when breaking from a loop
			map_iter_stop(a);
			break;
		}
	}

	// the map will reset everything IF you do not exit early from a loop, so
	// this is ok:

	while(map_has_next(a)) {
		const char* key = map_key(a);
		MyStruct* value = map_value(a);
	}

	while(map_has_next(a)) {
		const char* key = map_key(a);
		MyStruct* value = map_value(a);
	}

	while(map_has_next(a)) {
		const char* key = map_key(a);
		MyStruct* value = map_value(a);
	}

	map_free(a);

	// map_load can be used as a shorthand for initializing a map
	// initializer list should be array of `struct {const char* k; V v;}`

	map(MyStruct) c = map_new(MyStruct, MyStruct_free);
	map_insert_from(c, MyStruct, {
		{"abc", MyStruct_new("5", 5)},
		{"def", MyStruct_new("6", 6)},
		{"ghi", MyStruct_new("7", 7)},
	});

	while(map_has_next(c)) {
		const char* key = map_key(c);
		MyStruct* value = map_value(c);
		printf("%s : %d\n", key, value->x);
	}

	map_free(b);
	map_free(c);
	
	return 0;
}
