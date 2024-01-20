#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BREAKPOINT()                \
    printf("breakpoint @ %s:%d\n",  \
    (strrchr((__FILE__), '/')       \
    ? strrchr((__FILE__), '/') + 1  \
    : (__FILE__)), __LINE__)

#define MAP_MAX_LOAD 		0.75
#define MAP_RESIZE_FACTOR 	3
#define MAP_INITIAL_CAP 	10

#define MAP_FLAG_OPEN	 	0
#define MAP_FLAG_DELETED	1
#define MAP_FLAG_FULL		2

// #define map(V) int*

typedef void* map;

// typedef struct {
	// size_t 	iter_index;	// <-- hptr
	// size_t 	elemsize;
	// size_t 	size;
	// size_t 	cap;
	// int* 	values; // <-- ptr that is passed around
	// char** 	keys;
	// size_t*	flags;
// } map;

// public interface

#define map_new() \
	f_map_new()

#define map_new_size(initial_cap) \
	f_map_new_size(initial_cap)

#define map_copy(m) \
	f_map_copy(m)

#define map_free(m) \
	f_map_free(m)

#define map_contains(m, k) \
	f_map_contains(m, k)

#define map_get(m, k) \
	f_map_get(m, k)

#define map_set(m, k, v) \
	f_map_set(m, k, v)

#define map_delete(m, k) \
	f_map_delete(m, k)

#define map_iter_start(m) \
	f_map_iter_start(m)

#define map_iter_has_next(m) \
	f_map_iter_has_next(m)

#define map_iter_next_key(m) \
	f_map_iter_next_key(m)

// functions

uint32_t	f_map_str_hash(const char* str);

map		 	f_map_new();
map 		f_map_new_size(size_t initial_cap);
map 		f_map_copy(map m);
void 		f_map_free(map m);

bool		f_map_contains(map m, char* k);
int* 		f_map_get(map m, char* k);
void 		f_map_set(map m, char* k, int v);
void		f_map_delete(map m, char* k);

void		f_map_iter_start(map m);
bool		f_map_iter_has_next(map m);
char*		f_map_iter_next_key(map m);

void 		f_map_rehash(map m, size_t new_cap);

// internal macros for getting map fields from the dptr

#define map_iter_index(dptr) \
	(*(((size_t*) (dptr)) - 4))

#define map_elemsize(dptr) \
	(*(((size_t*) (dptr)) - 3))

#define map_size(dptr) \
	(*(((size_t*) (dptr)) - 2))

#define map_cap(dptr) \
	(*(((size_t*) (dptr)) - 1))

#define map_values(dptr) \
	(dptr)

#define map_keys(dptr) \
	(*(((char***) (dptr)) + 1))

#define map_flags(dptr) \
	(*(((size_t**) (dptr)) + 2))

// implementation

uint32_t f_map_str_hash(const char *str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

map f_map_new() {
    return f_map_new_size(MAP_INITIAL_CAP);
}

map f_map_new_size(size_t initial_cap) {

	void* m = malloc(4 * sizeof(size_t) + 3 * sizeof(void*));
    map_iter_index(m) = 0;
    map_elemsize(m) = sizeof(int);
    map_cap(m) = initial_cap;
    map_keys(m) = malloc(initial_cap * sizeof(char*));
    map_values(m) = malloc(initial_cap * sizeof(int));
    map_flags(m) = malloc(initial_cap * sizeof(size_t));

    for (size_t i = 0; i < initial_cap; ++i) {
    	map_flags(m)[i] = MAP_FLAG_OPEN;
    	map_keys(m)[i] = NULL;
    }

    return m;
}

// deep copies keys
map f_map_copy(map m) {

	void* m2 = malloc(4 * sizeof(size_t) + 3 * sizeof(void*));
    map_iter_index(m2) = 0;
    map_elemsize(m2) = map_elemsize(m);
    map_cap(m2) = map_cap(m);
    map_keys(m2) = malloc(map_cap(m) * sizeof(char*));
    map_values(m2) = malloc(map_cap(m) * sizeof(int));
    map_flags(m2) = malloc(map_cap(m) * sizeof(size_t));

	for (size_t i = 0; i < map_cap(m); i++) {
		if (map_flags(m)[i] == MAP_FLAG_FULL) {
			map_keys(m2)[i] = strdup(map_keys(m2)[i]);
		}
	}

    memcpy(map_values(m2), map_values(m), map_cap(m) * map_elemsize(m));
    memcpy(map_flags(m2), map_flags(m), map_cap(m) * sizeof(size_t));

    return m2;
}

void f_map_free(map m) {
    for (size_t i = 0; i < map_cap(m); ++i) {
    	if (map_flags(m)[i] == MAP_FLAG_FULL) {
        	free(map_keys(m)[i]);
    	}
    }
    free(map_flags(m));
    free(map_keys(m));
    free(map_values(m));
    free(m);
}

bool f_map_contains(map m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % map_cap(m);

    for (size_t i = 0; i < map_cap(m); ++i) {
        if (map_flags(m)[index] == MAP_FLAG_FULL
        && !strcmp(map_keys(m)[index], k)) {
            return true;
        } else if (map_flags(m)[index] == MAP_FLAG_OPEN) {
            return false;
        }

        index++;
        index = index % map_cap(m);
	}

    return false;
}

int* f_map_get(map m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % map_cap(m);

    for (size_t i = 0; i < map_cap(m); ++i) {
        if (map_flags(m)[index] == MAP_FLAG_FULL
        && !strcmp(map_keys(m)[index], k)) {
            return &((int*)m)[index];
        } else if (map_flags(m)[index] == MAP_FLAG_OPEN) {
            return NULL;
        }

        index++;
        index = index % map_cap(m);
	}

    return NULL;
}

void f_map_set(map m, char* k, int v) {
	uint32_t key_hash = f_map_str_hash(k);
	size_t index = key_hash % map_cap(m);

	// rehash?
	if (map_size(m) > MAP_MAX_LOAD * map_cap(m)) {
		size_t new_cap = MAP_RESIZE_FACTOR * map_cap(m);
		f_map_rehash(m, new_cap);
		index = key_hash % new_cap;
	}

	for (size_t i = 0; i < map_cap(m); i++) {
		size_t flag = map_flags(m)[index];
		if (flag == MAP_FLAG_OPEN
			|| flag == MAP_FLAG_DELETED
			|| (flag == MAP_FLAG_FULL && !strcmp(map_keys(m)[index], k)))
		{
			map_flags(m)[index] = MAP_FLAG_FULL;
			free(map_keys(m)[index]);
			map_keys(m)[index] = strdup(k); // freed in delete()
			((int*)m)[index] = v;
			map_size(m)++;
			return;
		}
		index++;
		index = index % map_cap(m);
	}
}

void f_map_delete(map m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % map_cap(m);

    for (size_t i = 0; i < map_cap(m); ++i) {
        if (map_flags(m)[index] == MAP_FLAG_FULL
        && strcmp(map_keys(m)[index], k) == 0){
            map_flags(m)[index] = MAP_FLAG_DELETED;
            map_size(m)--;
            free(map_keys(m)[index]);
            return;
        } else if (map_flags(m)[index] == MAP_FLAG_OPEN) {
            return;
        }

        index++;
        index = index % map_cap(m);
	}
}

void f_map_iter_start(map m) {
	map_iter_index(m) = 0;
}

bool f_map_iter_has_next(map m) {
	if (map_iter_index(m) >= map_cap(m)) {
		return false;
	}
	
	// lookahead
	for (size_t i = map_iter_index(m); i < map_cap(m); i++) {
		if (map_flags(m)[i] == MAP_FLAG_FULL) {
			return true;
		}
	}
	
	return false;
}

char* f_map_iter_next_key(map m) {
    if (!f_map_iter_has_next(m)) {
        return NULL;
    }
    
    while(map_flags(m)[map_iter_index(m)] != MAP_FLAG_FULL) {
    	map_iter_index(m)++;
    }

   	map_iter_index(m)++;
    return map_keys(m)[map_iter_index(m) - 1];
}

void f_map_rehash(map m, size_t new_cap) {
   	size_t* new_flags = malloc(new_cap * sizeof(size_t));
    char** new_keys = malloc(new_cap * sizeof(char*));
   	int* new_values = malloc(new_cap * sizeof(int));
   	
    for (size_t i = 0; i < new_cap; ++i) {
    	new_flags[i] = MAP_FLAG_OPEN;
    	new_keys[i] = NULL;
    }

    for (size_t i = 0; i < map_cap(m); ++i) {
        if (map_flags(m)[i] == MAP_FLAG_FULL) {
            size_t index = f_map_str_hash(map_keys(m)[i]) % new_cap;
            while (new_flags[index] == MAP_FLAG_FULL) {
				index++;
				index = index % new_cap;
            }

            new_flags[index] = map_flags(m)[i];
            new_keys[index] = map_keys(m)[i];
            new_values[index] = ((int*)m)[i];
        }
    }

    free(map_flags(m));
    free(map_keys(m));
    free(map_values(m));
    map_flags(m) = new_flags;
    map_keys(m) = new_keys;
    map_values(m) = new_values;
    map_cap(m) = new_cap;
}

void f_map_print(map m) {
	int full_slots = 0;
	printf("map<size=%zu, cap=%zu>:\n", map_size(m), map_cap(m));
	for (size_t i = 0; i < map_cap(m); i++) {
		size_t flag = map_flags(m)[i];
		if (flag == MAP_FLAG_OPEN)
			printf("\t[%zu] <open>\n", i);
		else if (flag == MAP_FLAG_FULL) {
			printf("\t[%zu] <'%s':%d>\n", i, map_keys(m)[i], ((int*)m)[i]);
			full_slots++;
		}
		else if (flag == MAP_FLAG_DELETED)
			printf("\t[%zu] <deleted>\n", i);
	}
	printf("(%d/%zu slots full)\n", full_slots, map_cap(m));
}

#endif // MAP_H
