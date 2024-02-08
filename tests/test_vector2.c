#include "../vector.h"

#include <assert.h>
#include <stdio.h>

#define N 100000

int main() {
    int* v = vector_new(int);

    printf("starting\n");

    for (int i = 0; i < N; i++) {
        vector_push(v, i);
    }

    for (int i = N-1; i >= 0; i--) {
        assert(vector_pop(v) == i);
    }

    assert(vector_is_empty(v));

    printf("---\n");

    for (int i = 0; i < N; i++) {
        vector_insert(v, i, i);
        // printf("i=%d, value=%d\n", i, v[vector_size(v)-1]);
    }

    for (int i = 0; i < N; i++) {
        int value = v[0];
        vector_remove(v, 0);
        // printf("i=%d, value=%d\n", i, value);
        assert(value == i);
    }

    assert(vector_is_empty(v));

    printf("done\n");

    vector_free(v);
}