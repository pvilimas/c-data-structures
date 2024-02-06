#include "../map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// test with a custom free function

typedef struct {
    void* p;
    int s;
} Data;

void free_data(void* p) {Data*d=p;printf("%d\n", d->s);free(d->p);}

int main() {

    map(Data) m = map_new(Data, free_data);

    map_insert(m, "a", (Data){.p=malloc(100), .s=100});
    map_insert(m, "ab", (Data){.p=malloc(300), .s=300});
    map_insert(m, "abc", (Data){.p=malloc(500), .s=500});
    map_insert(m, "abcd", (Data){.p=malloc(700), .s=700});

    map_remove(m, "abc");

    map_free(m);

    return 0;
}