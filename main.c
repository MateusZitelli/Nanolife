#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
//#include "SDL/SDL_gfxPrimitives.h"
#define SX 150
#define SY 150
#define WIDTH 150
#define HEIGHT 150
#define BPP 4
#define DEPTH 32

#define MEM_SIZE 32
#define MAX_AGE 1000.0

int last;
struct bot *bots;
/*
0 - ptr++
1 - ptr--
2 - memory[ptr]++
3 - memory[ptr]--
4 - while(memory[ptr]){
5 - }
6 - gcode[ptr] = 1 if have a bot in front or 0 if not
7 - Act;
*/

/*
1 - Rotate clockwise - dir++
2 - Rotate anticlockwise - dir--
3 - Move foward
4 - Move Back
5 - Reproduce
6 - Atack
7 - 
*/

struct bot {
	int p;
	float energy;
	short gcode[MEM_SIZE];
	short memory[MEM_SIZE];
	unsigned short nl;
	unsigned short loops[MEM_SIZE];
	unsigned short loops_ptr[MEM_SIZE];
	short ptr;
	short pos;
	unsigned short dir;
	int color;
	int age;
	int generation;
};

void set_bot(struct bot *b, int p, float e, unsigned short *g, struct bot **lb, short gen)
{
	short i;
	int cr = 0, cg = 0, cb = 0;
	if(p > SX * SY) printf("ups2\n");
	if (e <= 0)
		return;
	b->p = p;
	b->energy = e;
	for (i = 0; i < MEM_SIZE; i++) {
		b->gcode[i] = g[i];
		b->memory[i] = 0;
	}
	b->nl = 0;
	b->ptr = 0;
	b->pos = 0;
	b->dir = rand() % 4;
	for (i = 0; i < MEM_SIZE / 3; i++) {
		cr += g[i];
	}
	for (i = MEM_SIZE / 3; i < MEM_SIZE / 3 * 2; i++) {
		cg += g[i];
	}
	for (i = MEM_SIZE / 3 * 2; i < MEM_SIZE; i++) {
		cb += g[i];
	}
	cr = (int)((float)cr / (MEM_SIZE / 3 * 2) * 256) % 256;
	cg = (int)((float)cg / (MEM_SIZE / 3 * 2) * 256) % 256;
	cb = (int)((float)cb / (MEM_SIZE / 3 * 2) * 256) % 256;
	b->color = cr << 16 | cg << 8 | cb;
	b->age = MAX_AGE;
	b->generation = gen + 1;
}

char compatible(struct bot *b1, struct bot *b2)
{
	if (b1->gcode[MEM_SIZE - 1] == b2->gcode[MEM_SIZE - 1]
	    && b1->gcode[MEM_SIZE - 2] == b2->gcode[MEM_SIZE - 2] && b1->gcode[MEM_SIZE - 3] == b2->gcode[MEM_SIZE - 3] && b1->gcode[MEM_SIZE - 4] == b2->gcode[MEM_SIZE - 4]) {
		return (1);
	} else {
		return (0);
	}
}

void run(struct bot *b, struct bot **lb)
{
	//printf("%i -> %i\n", b->ptr, b->memory[b->ptr]);
	//b->energy -= 1;
	unsigned short ngcode[MEM_SIZE], i;
	int mean, p;
	switch (b->memory[b->ptr]) {
	case 1:
		b->dir = (b->dir + 1) % 4;
		break;
	case 2:
		b->dir = (b->dir - 1) % 4;
		break;
	case 3:
		//b->energy += 10;
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p + 1;
				lb[b->p] = b;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p - SX;
				lb[b->p] = b;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p ] = NULL;
				b->p = b->p - 1;
				lb[b->p] = b;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p + SX;
				lb[b->p] = b;
			}
			break;
		}
		break;
	case 4:
		//b->energy += 10;
		switch (b->dir) {
		case 2:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p + 1;
				lb[b->p] = b;
			}
			break;
		case 3:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p - SX;
				lb[b->p] = b;
			}
			break;
		case 0:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p - 1;
				lb[b->p] = b;
			}
			break;
		case 1:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p] = NULL;
				b->p = b->p + SX;
				lb[b->p] = b;
			}
			break;
		}
		break;
	case 5:
		//b->age += 10000;
		//b->energy += 100;
		for (i = 0; i < MEM_SIZE; i++) {
			ngcode[i] = b->gcode[i];
		}
		if (rand() % 1000 > 950)
			ngcode[rand() % MEM_SIZE] = rand() % 7;
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p + 1] = &bots[last];
				set_bot(&bots[last++], b->p + 1, b->energy / 5.0, ngcode, lb, b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p - SX] = &bots[last];
				set_bot(&bots[last++], b->p - SX, b->energy / 5.0, ngcode, lb, b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p - 1] = &bots[last];
				set_bot(&bots[last++], b->p - 1, b->energy / 5.0, ngcode, lb, b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p + SX] = &bots[last];
				set_bot(&bots[last++], b->p + SX, b->energy / 5.0, ngcode, lb, b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		}
		break;
	case 6:
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] != NULL) {
				b->energy += lb[b->p + 1]->energy / 3.0;
				lb[b->p + 1]->energy /= 2.0;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] != NULL) {
				b->energy += lb[b->p - SX]->energy / 3.0;
				lb[b->p - SX]->energy /= 2.0;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] != NULL) {
				b->energy += lb[b->p - 1]->energy / 3.0;
				lb[b->p - 1]->energy /= 2.0;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] != NULL) {
				b->energy += lb[b->p + SX]->energy / 3.0;
				lb[b->p + SX]->energy /= 2.0;
			}
			break;
		}
		break;
	case 7:
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] != NULL) {
				mean = (b->energy + lb[b->p + 1]->energy) / 2.0;
				b->energy = mean;
				lb[b->p + 1]->energy = mean;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] != NULL) {
				mean = (b->energy + lb[b->p - SX]->energy) / 2.0;
				b->energy = mean;
				lb[b->p - SX]->energy = mean;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] != NULL) {
				mean = (b->energy + lb[b->p - 1]->energy) / 2.0;
				b->energy = mean;
				lb[b->p - 1]->energy = mean;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] != NULL) {
				mean = (b->energy + lb[b->p + SX]->energy) / 2.0;
				b->energy = mean;
				lb[b->p + SX]->energy = mean;
			}
			break;
		}
		break;
	}
}

void compute(struct bot *b, struct bot **lb)
{
	//printf("1 %i -> %i\n", b->pos, b->gcode[b->pos]);
	--b->age;
	--b->energy;
	if (b->pos > MEM_SIZE || b->ptr > MEM_SIZE || b->ptr < 0 || b->nl > MEM_SIZE)
		return;
	switch (b->gcode[b->pos++]) {
	case 0:
		b->ptr++;
		break;
	case 1:
		b->ptr--;
		break;
	case 2:
		b->memory[b->ptr]++;
		break;
	case 3:
		b->memory[b->ptr]--;
		break;
	case 4:
		if (b->memory[b->ptr])
			b->loops[b->nl] = b->pos;
		b->loops_ptr[b->nl++] = b->ptr;
		break;
	case 5:
		if (b->nl && b->memory[b->loops_ptr[b->nl - 1]]) {
			b->pos = b->loops[b->nl - 1];
		} else {
			--b->nl;
		}
		break;
	case 6:
		run(b, lb);
		break;
	}
	b->memory[b->ptr] = b->memory[b->ptr] % 8;
}

void setpixel(SDL_Surface * screen, int x, int y, int r, int g, int b)
{
	if (x < 0 || y < 0 || x >= SX || y >= SY || r < 0 || g < 0 || b < 0)
		return;
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;
	Uint32 *pixmem32;
	Uint32 colour;

	colour = SDL_MapRGB(screen->format, r, g, b);

	pixmem32 = (Uint32 *) screen->pixels + screen->w * y + x;
	*pixmem32 = colour;
}

int main(void)
{
	srand(time(0));
	SDL_Surface *screen;
	SDL_Event event;
	last = 0;
	int keypress = 0;
	int h = 0, k = 0;
	long long b = 0, v = 0, a = MAX_AGE;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE))) {
		SDL_Quit();
		return 1;
	}
	int i, px, py, j;
	unsigned short g[MEM_SIZE];
	bots = (struct bot *)malloc(sizeof(struct bot) * SX * SY);
	struct bot **lb = (struct bot **)malloc(sizeof(struct bot *) * SX * SY);
	for (i = 0; i < SX * SY; i++) {
		lb[i] = NULL;
	}
	for (j = 0; j < 0; j++) {
		px = rand() % WIDTH;
		py = rand() % HEIGHT;
		for (i = 0; i < MEM_SIZE; i++) {
			g[i] = rand() % 7;
		}
		set_bot(&bots[last++], py * SX + px, 1000, g, lb, 0);
	}
	short get = 0, view = 0;
	while (!keypress) {
		for (i = 0; i < last; i++) {
			compute(&bots[i], lb);
			switch (view) {
			case 0:
				setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].color >> 16, (bots[i].color >> 8) & 0xFF, bots[i].color & 0xFF);
				break;
			case 1:
				setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].energy / 10.0, bots[i].energy / 100.0, bots[i].energy / 1000.0);
				break;
			case 2:
				setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].age / MAX_AGE * 255, bots[i].age / MAX_AGE * 255, bots[i].age / MAX_AGE * 255);
				break;
			case 3:
				setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].generation / 2.0, bots[i].generation / 2.0, bots[i].generation / 2.0);
				break;
			case 4:
				if(bots[i].generation > 1)
					setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].color >> 16, (bots[i].color >> 8) & 0xFF, bots[i].color & 0xFF);
				break;
			}
		}
		for (i = 0; i < last; i++) {
			if (get) {
				if (bots[i].energy > v && bots[i].generation > 20) {
					b = i;
					v = bots[i].energy;
				}
			}
			if (!bots[i].energy || !bots[i].age) {
				lb[bots[i].p] = NULL;
				bots[i] = bots[--last];
				lb[bots[i].p] = &bots[i];
			}
		}
		if (get) {
			for (i = 0; i < MEM_SIZE; i++) {
				printf("%i\n", bots[b].gcode[i]);
			}
			v = 0;
			printf("##################\n");
			get = 0; 
		}
		for (j = 0; j < 10; j++) {
			px = rand() % WIDTH;
			py = rand() % HEIGHT;
			for (i = 0; i < MEM_SIZE; i++) {
				g[i] = rand() % 7;
			}
			if (lb[py * SX + px] == NULL)
				set_bot(&bots[last++], py * SX + px, 1000, g, lb, 0);
		}
		if (view != 5 && k % 100 == 0) {
			SDL_Flip(screen);
			//if(k % 1000 == 0)
			SDL_FillRect(screen, NULL, 0x000000);
		}
		//k;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				keypress = 1;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_g:
					get = 1;
					break;
				case SDLK_v:
					view = (view + 1) % 6;
					switch (view) {
					case 0:
						printf("Genetic View\n");
						break;
					case 1:
						printf("Energy View\n");
						break;
					case 2:
						printf("Age View\n");
						break;
					case 3:
						printf("Generation View\n");
						break;
					case 4:
						printf("Filtered Genetic View\n");
						break;
					case 5:
						printf("Don't rendening\n");
						SDL_FillRect(screen, NULL, 0x000000);
						break;
					}
					break;
				}
				break;
			}
		}
	}
	return (0);
}
