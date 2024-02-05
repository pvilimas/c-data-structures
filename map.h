#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/*

	map.h - generic hashmap with string keys

	Keys must be const char* (string literals), value can be any type. Before 
	calling map_get, ensure the key is in the map (using map_contains).
	map_find will safely return NULL if the value is not present however.
	Iteration order is basically random. Uses linear probing. For more details
	look at map_example.c.

	Properties:

	map_size(m) -> size_t               -- Returns the size of the map

	Methods:

	map_new(V) -> map<const char*, V>   -- Create a new map
	map_free(m)                         -- Free all memory in the map
	map_insert(m, k, v) -> V            -- Insert a key-value pair into the map
	map_contains(m, k) -> bool          -- Is this key contained in the map?
	map_get(m, k) -> V                  -- Get the value (MUST BE IN THE MAP)
	map_find(m, k) -> V*                -- Get a ptr to the value (or NULL)
	map_remove(m, k)                    -- Remove the key from the map

	Iteration:

	map_has_next(m) -> bool             -- Are there any keys left to iterate?
	map_key(m) -> const char*			-- Get current key
	map_value(m) -> V					-- Get current value
	map_iter_stop(m)                    -- Call this function when using break

*/

#define MAP_MAX_LOAD 		0.75
#define MAP_INITIAL_CAP 	10
#define MAP_RESIZE_FACTOR 	4

#define MAP_FLAG_OPEN 		0
#define MAP_FLAG_DELETED 	1
#define MAP_FLAG_FULL		2

#define map(V) V**

#define map_size(vp) \
	(i_map_v2h((vp))->size)

#define map_new(V) \
	((V**) f_map_new(MAP_INITIAL_CAP, 0, sizeof(V)))

#define map_free(vp) \
	(f_map_free(i_map_v2h((vp))), (vp) = NULL)

#define map_insert(vp, k, ...) \
	(*(void***)(&(vp)) = (void**)i_map_h2v(f_map_try_rehash(i_map_v2h((vp)))), \
	(*(vp))[f_map_insert(i_map_v2h((vp)), (k))] = (__VA_ARGS__))

#define map_contains(vp, k) \
	(f_map_contains(i_map_v2h((vp)), (k)))

#define map_get(vp, k) \
	(*(vp))[f_map_get(i_map_v2h((vp)), (k))]

#define map_find(vp, k)													\
	((i_map_v2h((vp))->last_get_index = f_map_get(i_map_v2h((vp)), (k))),	\
	((i_map_v2h((vp))->last_get_index != -1)								\
		? ((*(vp)) + i_map_v2h((vp))->last_get_index)						\
		: NULL))

#define map_remove(vp, k) \
	(f_map_remove(i_map_v2h((vp)), (k)))

#define map_has_next(vp) \
	(f_map_has_next(i_map_v2h((vp))))

#define map_key(vp) \
	(i_map_v2h((vp))->keys[i_map_v2h((vp))->iter_index - 1])

#define map_value(vp) \
	((*(vp))[i_map_v2h((vp))->iter_index - 1])

#define map_iter_stop(vp) \
	(i_map_v2h((vp))->iter_index = 0, i_map_v2h((vp))->iter_last_error = false)

// memory layout
typedef struct {
	size_t          cap;
	size_t          size;
	size_t          vsize;
	size_t          iter_index;
	size_t			iter_last_error; // 1 if map_next last returned null
	int             last_get_index; // save result during map_find
	uint8_t*        flags;
	uint32_t*       hashes;
	const char**    keys;
	void*           values; // <-- vptr
} i_map_header;

// header* -> void**
#define i_map_h2v(hptr) \
	(void**)((char*)(hptr) + offsetof(i_map_header, values))

// V** -> header*
#define i_map_v2h(vptr) \
	((i_map_header*)((char*)(vptr) - offsetof(i_map_header, values)))

static inline uint32_t f_str_hash(const char* str) {
    uint32_t hash = 5381;
    int c;
    while ((c = *str++) != 0) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

static inline void** f_map_new(size_t cap, size_t size, size_t vsize) {
	i_map_header* hp = malloc(sizeof(i_map_header));
	*hp = (i_map_header) {
		.cap = cap,
		.size = size,
		.vsize = vsize,
		.iter_index = 0,
		.flags = calloc(cap, sizeof(uint8_t)),
		.hashes = calloc(cap, sizeof(uint32_t)),
		.keys = malloc(cap * sizeof(const char*)),
		.values = malloc(cap * vsize)
	};
	return i_map_h2v(hp);
}

static inline void f_map_free(i_map_header* hp) {
	free(hp->flags);
	free(hp->hashes);
	free(hp->keys);
	free(hp->values);
	free(hp);
}

// returns new ptr
static inline i_map_header* f_map_try_rehash(i_map_header* hp) {
	if (hp->size < MAP_MAX_LOAD * hp->cap) {
		return hp;
	}

    size_t new_cap = hp->cap * MAP_RESIZE_FACTOR;
	i_map_header* new_hp = malloc(sizeof(i_map_header));
	*new_hp = (i_map_header) {
		.cap = new_cap,
		.size = hp->size,
		.vsize = hp->vsize,
		.iter_index = 0,
		.flags = calloc(new_cap, sizeof(uint8_t)),
		.hashes = calloc(new_cap, sizeof(uint32_t)),
		.keys = malloc(new_cap * sizeof(const char*)),
		.values = malloc(new_cap * hp->vsize)
	};

    for (size_t i = 0; i < hp->cap; i++) {
        if (hp->flags[i] != MAP_FLAG_FULL) {
            continue;
        }

        uint32_t k_hash = hp->hashes[i];
        size_t index = k_hash % new_cap;

        while (new_hp->flags[index] == MAP_FLAG_FULL) {
            index = (index + 1) % new_cap;
        }

        new_hp->flags[index] = hp->flags[i];
        new_hp->hashes[index] = hp->hashes[i];
        new_hp->keys[index] = hp->keys[i];
        // new_values[index] = hp->values[i];
        memcpy(
        	((char*)new_hp->values) + (index * hp->vsize),
        	((char*)hp->values) + (i * hp->vsize),
        	hp->vsize
        );
    }

	free(hp->flags);
	free(hp->hashes);
	free(hp->keys);
	free(hp->values);
	free(hp);
    return new_hp;
}

// returns index, value gets set outside
static inline size_t f_map_insert(i_map_header* hp, const char* k) {
	hp->iter_index = 0;

    uint32_t k_hash = f_str_hash(k);
    size_t index = k_hash % hp->cap;

    while (hp->flags[index] == MAP_FLAG_FULL) {
        if (hp->hashes[index] == k_hash) {
            // replace existing
            // hp->values[index] = v;
            return index;
        }

        index = (index + 1) % hp->cap;
    }

	// insert new
    hp->size++;
	hp->flags[index] = MAP_FLAG_FULL;
	hp->hashes[index] = k_hash;
	hp->keys[index] = k;
	// hp->values[index] = v;
	return index;
}

static inline bool f_map_contains(i_map_header* hp, const char* k) {
	if (hp->size == 0) {
		return false;
	}
	
	uint32_t k_hash = f_str_hash(k);
	size_t index = k_hash % hp->cap;

	while(hp->flags[index] != MAP_FLAG_OPEN) {
		if (hp->flags[index] == MAP_FLAG_FULL
		&& hp->hashes[index] == k_hash) {
			return true;
		}
		index = (index + 1) % hp->cap;
	}

	return false;
}

// returns index of key or -1
static inline int f_map_get(i_map_header* hp, const char* k) {
	if (hp->size == 0) {
		return -1;
	}

	uint32_t k_hash = f_str_hash(k);
	size_t index = k_hash % hp->cap;

	while(hp->flags[index] != MAP_FLAG_OPEN) {
		if (hp->flags[index] == MAP_FLAG_FULL
		&& hp->hashes[index] == k_hash) {
			return index;
		}
		index = (index + 1) % hp->cap;
	}

	return -1;
}

static inline void f_map_remove(i_map_header* hp, const char* k) {
	if (hp->size == 0) {
		return;
	}

	hp->iter_index = 0;

	uint32_t k_hash = f_str_hash(k);
	size_t index = k_hash % hp->cap;

	while(hp->flags[index] != MAP_FLAG_OPEN) {
		if (hp->flags[index] == MAP_FLAG_FULL
		&& hp->hashes[index] == k_hash) {
	        hp->flags[index] = MAP_FLAG_DELETED;
	        hp->size--;
			return;
		}
		index = (index + 1) % hp->cap;
	}
}

// set iter_index and last_error
static inline bool f_map_has_next(i_map_header* hp) {
	if (hp->iter_last_error) {
		hp->iter_index = 0;
		hp->iter_last_error = false;
	}

	if (hp->size == 0 || hp->iter_index >= hp->cap) {
		hp->iter_last_error = true;
		return false;
	}

	for (size_t i = hp->iter_index; i < hp->cap; i++) {
		if (hp->flags[i] == MAP_FLAG_FULL) {
			hp->iter_index = i + 1;
			hp->iter_last_error = false;
			return true;
		}
	}

	hp->iter_last_error = true;
	return false;
}

#endif // MAP_H
