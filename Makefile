all:
	gcc -O0 -msse -mmmx -g -W -ftree-vectorize main.c -o b.bin `sdl-config --cflags` `sdl-config --libs`
