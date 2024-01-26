all: build run

build:
	gcc -std=c99 -Wall -Wextra -o build/test_map tests/test_map.c

run:
	./build/test_map 

vg:
	valgrind --track-origins=yes ./build/test_map
