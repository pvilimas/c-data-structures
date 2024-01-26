# c-data-structures
- Simple generic data structures written in C99
	- array.h: static array
	- vector.h: dynamic array
	- map.h: hashmap with string keys
- How to use
	just copy and paste the header files you need into your folder and include them
- What's useful about these?
	- They only have the minimal functions you would need, anything else can be done by looping through them
	- No need for `init_vector(T);` style macros
	- In vector and array you can index elements directly just like a pointer, no need for `vec_set(v, 0, 5)` stuff either - `v[0] = 5`
	- Fully generic, you can put whatever you want in them, not limited to storing pointers
