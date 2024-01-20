#include "array.h"

typedef struct {
	int x;
	char* c;
} st;

char* strings[] = {
	[0] = "a",
	[1] = "b",
	[2] = "c"
};

int main() {
	array(st) a = array_new(st, 3);
	for (int i = 0; i < (int) array_size(a); i++) {
		a[i] = (st){ .x = i, .c = strings[i] };
	}
	
	for (int i = 0; i < (int) array_size(a); i++) {
		a[i].x += 17;
		printf("%d: %s\n", a[i].x, a[i].c);
	}
	printf("size: %zu\n", array_size(a));
	array_free(a);

	printf("done\n");
	return 0;
}
