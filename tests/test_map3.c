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

    map(Data) m2 = map_new(Data, free_data);
    map_insert_from(m2, Data, {
        {"a", (Data){.p=malloc(100), .s=100}},
        {"ab", (Data){.p=malloc(300), .s=300}},
        {"abc", (Data){.p=malloc(500), .s=500}},
        {"abcd", (Data){.p=malloc(700), .s=700}}
    });

    while (map_has_next(m2)) {
        printf("%s : %d\n", map_key(m2), map_value(m2)->s);
    }

    map_free(m2);

    return 0;
}