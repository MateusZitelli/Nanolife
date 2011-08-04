all:
	gcc -march=amdfam10 -O2 -pipe -g -W main.c -o b.bin `sdl-config --cflags` `sdl-config --libs`
