#include "../vector.h"

#include <stdio.h>

typedef struct {
	char* s;
	int x;
} MyStruct;

int main() {

	// these are both valid declarations:
	vector(MyStruct) a = vector_new(MyStruct);
	MyStruct* b = vector_new(MyStruct);

	// but this will not work:
	// void* c = vector_new(MyStruct);

	// push and pop can be used like you'd expect
	// elements can be indexed directly, just like a pointer:
	printf("value pushed: %s\n", vector_push(a, (MyStruct){ .s = "hi" }).s);
	a[0].s = "bye";
	printf("value popped: %s\n", vector_pop(a).s);

	for (int i = 0; i < 10; i++) {
		vector_push(a, (MyStruct) { .x = i });
	}

	// vector_is_empty can be used in a loop:
	while(!vector_is_empty(a)) {
		printf("%d\n", vector_pop(a).x);
	}

	// to iterate over the vector, the vector_size macro can be used:
	for (int i = 0; i < (int)vector_size(a); i++) {
		a[i] = (MyStruct){ .s = "abcdef" + i, .x = i };
	}

	vector_free(a);
	vector_free(b);

	return 0;
}
