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
// typedef void* map;

typedef struct {
	size_t 	iter_index;	// <-- hptr
	size_t 	elemsize;
	size_t 	size;
	size_t 	cap;
	int* 	values; // <-- ptr that is passed around
	char** 	keys;
	size_t*	flags;
} map;

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
map 		f_map_copy(map* m);
void 		f_map_free(map* m);

bool		f_map_contains(map* m, char* k);
int* 		f_map_get(map* m, char* k);
void 		f_map_set(map* m, char* k, int v);
void		f_map_delete(map* m, char* k);

void		f_map_iter_start(map* m);
bool		f_map_iter_has_next(map* m);
char*		f_map_iter_next_key(map* m);

void 		f_map_rehash(map* m, size_t new_cap);

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

	map m = {
    	.iter_index = 0,
    	.elemsize = sizeof(int),
    	.cap = initial_cap,
    	.keys = malloc(initial_cap * sizeof(char*)),
    	.values = malloc(initial_cap * sizeof(int)),
    	.flags = malloc(initial_cap * sizeof(size_t))
	};

    for (size_t i = 0; i < initial_cap; ++i) {
    	m.flags[i] = MAP_FLAG_OPEN;
    	m.keys[i] = NULL;
    }

    return m;
}

// deep copies keys
map f_map_copy(map* m) {

	map m2 = {
		.iter_index = 0,
		.elemsize = m->elemsize,
		.cap = m->cap,
		.keys = malloc(m->cap * sizeof(char*)),
		.values = malloc(m->cap * sizeof(int)),
		.flags = malloc(m->cap * sizeof(size_t))	
	};

	for (size_t i = 0; i < m->cap; i++) {
		if (m2.flags[i] == MAP_FLAG_FULL) {
			m2.keys[i] = strdup(m2.keys[i]);
		}
	}

    memcpy(m2.values, m->values, m->cap * m->elemsize);
    memcpy(m2.flags, m->flags, m->cap * sizeof(size_t));

    return m2;
}

void f_map_free(map* m) {
    for (size_t i = 0; i < m->cap; ++i) {
    	if (m->flags[i] == MAP_FLAG_FULL) {
        	free(m->keys[i]);
    	}
    }
    free(m->flags);
    free(m->keys);
    free(m->values);
    free(m);
}

bool f_map_contains(map* m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % m->cap;

    for (size_t i = 0; i < m->cap; ++i) {
        if (m->flags[index] == MAP_FLAG_FULL
        && !strcmp(m->keys[index], k)) {
            return true;
        } else if (m->flags[index] == MAP_FLAG_OPEN) {
            return false;
        }

        index++;
        index = index % m->cap;
	}

    return false;
}

int* f_map_get(map* m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % m->cap;

    for (size_t i = 0; i < m->cap; ++i) {
        if (m->flags[index] == MAP_FLAG_FULL
        && !strcmp(m->keys[index], k)) {
            return &((int*)m)[index];
        } else if (m->flags[index] == MAP_FLAG_OPEN) {
            return NULL;
        }

        index++;
        index = index % m->cap;
	}

    return NULL;
}

void f_map_set(map* m, char* k, int v) {
	uint32_t key_hash = f_map_str_hash(k);
	size_t index = key_hash % m->cap;

	// rehash?
	if (m->size > MAP_MAX_LOAD * m->cap) {
		size_t new_cap = MAP_RESIZE_FACTOR * m->cap;
		f_map_rehash(m, new_cap);
		index = key_hash % new_cap;
	}

	for (size_t i = 0; i < m->cap; i++) {
		size_t flag = m->flags[index];
		if (flag == MAP_FLAG_OPEN
			|| flag == MAP_FLAG_DELETED
			|| (flag == MAP_FLAG_FULL && !strcmp(m->keys[index], k)))
		{
			m->flags[index] = MAP_FLAG_FULL;
			free(m->keys[index]);
			m->keys[index] = strdup(k); // freed in delete()
			m->values[index] = v;
			m->size++;
			return;
		}
		index++;
		index = index % m->cap;
	}
}

void f_map_delete(map* m, char* k) {
    uint32_t key_hash = f_map_str_hash(k);
    size_t index = key_hash % m->cap;

    for (size_t i = 0; i < m->cap; ++i) {
        if (m->flags[index] == MAP_FLAG_FULL
        && strcmp(m->keys[index], k) == 0){
            m->flags[index] = MAP_FLAG_DELETED;
            m->size--;
            free(m->keys[index]);
            return;
        } else if (m->flags[index] == MAP_FLAG_OPEN) {
            return;
        }

        index++;
        index = index % m->cap;
	}
}

void f_map_iter_start(map* m) {
	map_iter_index(m) = 0;
}

bool f_map_iter_has_next(map* m) {
	if (map_iter_index(m) >= m->cap) {
		return false;
	}
	
	// lookahead
	for (size_t i = map_iter_index(m); i < m->cap; i++) {
		if (m->flags[i] == MAP_FLAG_FULL) {
			return true;
		}
	}
	
	return false;
}

char* f_map_iter_next_key(map* m) {
    if (!f_map_iter_has_next(m)) {
        return NULL;
    }
    
    while(m->flags[map_iter_index(m)] != MAP_FLAG_FULL) {
    	map_iter_index(m)++;
    }

   	map_iter_index(m)++;
    return m->keys[map_iter_index(m) - 1];
}

void f_map_rehash(map* m, size_t new_cap) {
   	size_t* new_flags = malloc(new_cap * sizeof(size_t));
    char** new_keys = malloc(new_cap * sizeof(char*));
   	int* new_values = malloc(new_cap * sizeof(int));
   	
    for (size_t i = 0; i < new_cap; ++i) {
    	new_flags[i] = MAP_FLAG_OPEN;
    	new_keys[i] = NULL;
    }

    for (size_t i = 0; i < m->cap; ++i) {
        if (m->flags[i] == MAP_FLAG_FULL) {
            size_t index = f_map_str_hash(m->keys[i]) % new_cap;
            while (new_flags[index] == MAP_FLAG_FULL) {
				index++;
				index = index % new_cap;
            }

            new_flags[index] = m->flags[i];
            new_keys[index] = m->keys[i];
            new_values[index] = m->values[i];
        }
    }

    free(m->flags);
    free(m->keys);
    free(m->values);
    m->flags = new_flags;
    m->keys = new_keys;
    m->values = new_values;
    m->cap = new_cap;
}

void f_map_print(map* m) {
	int full_slots = 0;
	printf("map<size=%zu, cap=%zu>:\n", m->size, m->cap);
	for (size_t i = 0; i < m->cap; i++) {
		size_t flag = m->flags[i];
		if (flag == MAP_FLAG_OPEN)
			printf("\t[%zu] <open>\n", i);
		else if (flag == MAP_FLAG_FULL) {
			printf("\t[%zu] <'%s':%d>\n", i, m->keys[i], ((int*)m)[i]);
			full_slots++;
		}
		else if (flag == MAP_FLAG_DELETED)
			printf("\t[%zu] <deleted>\n", i);
	}
	printf("(%d/%zu slots full)\n", full_slots, m->cap);
}

#endif // MAP_H
