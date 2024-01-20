#include "map.h"

#include <stdio.h>
#include <assert.h>

void assert_true(bool c, char* str) {
	if (!c) {
		printf("%s\n", str);
		exit(1);
	}
}

int main() {
	map m = map_new();

	char key[20];
	for (int i = 0; 10; i++) {
		sprintf(key, "key%d", i);
		map_set(&m, key, i * 2);
	}
// 
	// for (int i = 0; 10; i++) {
		// sprintf(key, "key%d", i);
		// assert_true(map_contains(&m, key), "contains");
		// assert_true(*map_get(&m, key) == i * 2, "get");
	// }

	map_free(&m);
	return 0;
}
