#include "../array.h"

#include <stdio.h>

typedef struct {
	char* s;
	int x;
} MyStruct;

int main() {

	// these are both valid declarations:
	array(MyStruct) a = array_new(MyStruct, 5);
	MyStruct* b = array_new(MyStruct, 5);

	// but this will not work:
	// void* c = array_new(MyStruct, 5);

	// elements can be indexed directly, just like a pointer:
	a[3] = (MyStruct){ .s = "hi", .x = 9 };
	printf("%d\n", a[3].x);

	// to iterate over the array, the array_size macro can be used:
	// (note that this returns size_t, casted it here to avoid compiler warning)
	for (int i = 0; i < (int)array_size(a); i++) {
		b[i] = (MyStruct){ .s = "abcdef" + i, .x = i };
		printf("%d: %s\n", b[i].x, b[i].s);
	}

	array_free(a);
	array_free(b);

	return 0;
}
