all:
	gcc -O3 -pipe -g -W main.c -o b.bin `sdl-config --cflags` `sdl-config --libs`

