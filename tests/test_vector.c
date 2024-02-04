#include "../vector.h"

typedef struct {
	int x;
	char* c;
} st;

char* strings[] = {
	[0] = "a",
	[1] = "b",
	[2] = "c",
	[3] = "d",
	[4] = "e",
	[5] = "f",
	[6] = "g",
	[7] = "h",
	[8] = "i",
	[9] = "j",
	[10] = "k",
};

int main() {
	vector(st) v = vector_new(st);

	for (int i = 0; i < 10; i++) {
		st s = (st){i+22,strings[i]};
		vector_push(v, s);
		printf("size: %zu, val: %d, str: %s\n", vector_size(v), s.x, s.c);
	}

	for (int i = 0; i < 5; i++) {
		st s = (st){99,"abc"};
		vector_insert(v, 3, s);
		// printf("size: %zu, val: %d, str: %s\n", vector_size(v), s.x, s.c);
	}

	printf("---\n");

	while (!vector_is_empty(v)) {
		st s = v[0];
		printf("size: %zu, val: %d, str: %s\n", vector_size(v), s.x, s.c);
		vector_remove(v, 0);
	}
	
	vector_free(v);
	printf("done\n");
	return 0;
}
