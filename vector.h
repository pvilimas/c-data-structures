#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*

	This is a very simple vector class in C. Only the basic functions are
	provided and there is no error checking or bounds checking (for example if
	you call vector_pop or access v[5] when the vector is empty). There are no
	iterator functions or anything like that, just use a loop with vector_size().

	The type vector(T) is defined as T* so you can read and write elements
	directly, see vector_example.c.

	Properties:

	vector_size(v) -> size_t		-- Returns the size of the vector
	vector_cap(v) -> size_t			-- Returns the capacity of the vector
	vector_elemsize(v) -> size_t	-- Returns sizeof(T)

	Methods:

	vector_new(T) -> vector(T)		-- Creates a new vector
	vector_free(v)					-- Frees all memory in the vector
	vector_push(v, T) -> T 			-- Push a value and return it
	vector_pop(v) -> T 				-- Pop a value and return it
	vector_is_empty(v) -> bool		-- Is the vector empty?
	
*/

#define VECTOR_RESIZE_FACTOR 2

#define vector(T) T*

#define vector_elemsize(dptr) \
	(*(((size_t*) (dptr)) - 3))

#define vector_size(dptr) \
	(*(((size_t*) (dptr)) - 2))

#define vector_cap(dptr) \
	(*(((size_t*) (dptr)) - 1))

#define vector_new(T) \
	((T*) f_vector_new(sizeof(T), 0, 10))

#define vector_free(v) \
	f_vector_free((void*)(v))

// using __VA_ARGS__ here allows compound initializer to be used
#define vector_push(v, ...) 		\
	(vector_size((v))++, 			\
	(v) = f_vector_try_resize((v)), \
	(v)[vector_size((v))-1] = (__VA_ARGS__))

#define vector_pop(v) \
	(vector_size((v))--, (v)[vector_size((v))])

#define vector_is_empty(v) \
	(vector_size((v)) == 0)

// MEMORY LAYOUT:
// size_t elemsize  <-- hptr points here
// size_t size
// size_t cap
// T*	  dptr		<-- the pointer given to the user

// internal macros for converting between dptr and hptr
// dptr - data pointer, only contains the data
// hptr - header pointer, contains header and data

#define i_vector_d2h(dptr) \
	(((size_t*) (dptr)) - 3)

#define i_vector_h2d(hptr) \
	((void*)(((size_t*) (hptr)) + 3))

// internal helper functions

static inline void* f_vector_new(size_t esz, size_t sz, size_t cap) {
	// allocate header + data
	size_t total_size = (sizeof(size_t) * 3) + (esz * cap);
	size_t* hptr = malloc(total_size);
    hptr[0] = esz;
    hptr[1] = sz;
    hptr[2] = cap;
    return i_vector_h2d(hptr); // return dptr
}

static inline void f_vector_free(void* dptr) {
	free(i_vector_d2h(dptr)); 
}

// double capacity if size = capacity
static inline void* f_vector_try_resize(void* dptr) {
	if (vector_size(dptr) == vector_cap(dptr)) {
		vector_cap(dptr) *= VECTOR_RESIZE_FACTOR;
		void* hptr = i_vector_d2h(dptr);
		size_t total_size = (sizeof(size_t) * 3) + 
			vector_elemsize(dptr) * vector_cap(dptr);
		hptr = realloc(hptr, total_size);
		return i_vector_h2d(hptr);
	}
	return dptr;
}

#endif // VECTOR_H