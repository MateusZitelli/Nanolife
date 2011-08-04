#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
//#include "SDL/SDL_gfxPrimitives.h"
#define SX 300
#define SY 300
#define WIDTH 300
#define HEIGHT 300
#define BPP 4
#define DEPTH 32

#define MEM_SIZE 128
#define MAX_AGE 1000.0

int last;
struct bot *bots;

/*
0x0 - ptr++
0x1 - ptr--
0x2 - memory[b->ptr]++
0x3 - memory[b->ptr]--
0x4 - while(memory[b->ptr]){
0x5 - }
0x6 - gcode[b->ptr] = 1 if have a bot in front or 0 if not
0x7 - Act;
*/

/*
0x0 - Do nothing
0x1 - Rotate clockwise - dir++
0x2 - Rotate anticlockwise - dir--
0x3 - Move foward
0x4 - Move Back
0x5 - Reproduce
0x6 - Atack
0x7 - Divide energy
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
	short r;
	short g;
	short b;
	int age;
	int generation;
};

    /* The Itoa code is in the puiblic domain */
char *itoa(int value, char *str, int radix)
{
	static char dig[] = "0123456789" "abcdefghijklmnopqrstuvwxyz";
	int n = 0, neg = 0;
	unsigned int v;
	char *p, *q;
	char c;

	if (radix == 10 && value < 0) {
		value = -value;
		neg = 1;
	}
	v = value;
	do {
		str[n++] = dig[v % radix];
		v /= radix;
	} while (v);
	if (neg)
		str[n++] = '-';
	str[n] = '\0';

	for (p = str, q = p + (n - 1); p < q; ++p, --q)
		c = *p, *p = *q, *q = c;
	return str;
}

void
set_bot(struct bot *b, int p, float e, unsigned short *g, struct bot **lb,
	short gen)
{
	short i;
	int cr = 0, cg = 0, cb = 0;
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
	b->r = ((int)((float)cr / (MEM_SIZE / 3 * 8) * 256) % 256 - 64) * 2;
	b->g = ((int)((float)cg / (MEM_SIZE / 3 * 8) * 256) % 256 - 64) * 2;
	b->b = ((int)((float)cb / (MEM_SIZE / 3 * 8) * 256) % 256 - 64) * 2;
	b->age = MAX_AGE;
	b->generation = gen + 1;
}

char compatible(struct bot *b1, struct bot *b2)
{
	int pos, i;
	for (i = 0; i < 4; i++) {
		pos = rand() % MEM_SIZE;
		if (b1->gcode[pos] != b2->gcode[pos])
			return (0);
	}
	return (1);
}

void run(struct bot *b, struct bot **lb)
{
	--b->energy;
	//printf("%i -> %i\n", b->ptr, b->memory[b->ptr]);
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
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p] = NULL;
				lb[b->p + 1] = b;
				b->p = b->p + 1;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p] = NULL;
				lb[b->p - SX] = b;
				b->p = b->p - SX;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p] = NULL;
				lb[b->p - 1] = b;
				b->p = b->p - 1;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p] = NULL;
				lb[b->p + SX] = b;
				b->p = b->p + SX;
			}
			break;
		}
		break;
	case 4:
		switch (b->dir) {
		case 2:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p] = NULL;
				lb[b->p + 1] = b;
				b->p = b->p + 1;
			}
			break;
		case 3:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p] = NULL;
				lb[b->p - SX] = b;
				b->p = b->p - SX;
			}
			break;
		case 0:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p] = NULL;
				lb[b->p - 1] = b;
				b->p = b->p - 1;
			}
			break;
		case 1:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p] = NULL;
				lb[b->p + SX] = b;
				b->p = b->p + SX;
			}
			break;
		}
		break;
	case 5:
		for (i = 0; i < MEM_SIZE; i++) {
			ngcode[i] = b->gcode[i];
		}
		for(i = 0; i < 100; i++){
			if (rand() % 1000 > 800)
				ngcode[rand() % MEM_SIZE] = rand() % 9;
			else
				break;
		}
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] == NULL) {
				lb[b->p + 1] = &bots[last];
				set_bot(&bots[last++], b->p + 1,
					b->energy / 5.0, ngcode, lb,
					b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] == NULL) {
				lb[b->p - SX] = &bots[last];
				set_bot(&bots[last++], b->p - SX,
					b->energy / 5.0, ngcode, lb,
					b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] == NULL) {
				lb[b->p - 1] = &bots[last];
				set_bot(&bots[last++], b->p - 1,
					b->energy / 5.0, ngcode, lb,
					b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] == NULL) {
				lb[b->p + SX] = &bots[last];
				set_bot(&bots[last++], b->p + SX,
					b->energy / 5.0, ngcode, lb,
					b->generation);
				b->energy -= b->energy / 5.0;
			}
			break;
		}
		break;
	case 6:
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] != NULL) {
				b->energy += lb[b->p + 1]->energy / 2.0;
				lb[b->p + 1]->energy /= 2.0;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] != NULL) {
				b->energy += lb[b->p - SX]->energy / 2.0;
				lb[b->p - SX]->energy /= 2.0;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] != NULL) {
				b->energy += lb[b->p - 1]->energy / 2.0;
				lb[b->p - 1]->energy /= 2.0;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] != NULL) {
				b->energy += lb[b->p + SX]->energy / 2.0;
				lb[b->p + SX]->energy /= 2.0;
			}
			break;
		}
		break;
	case 7:
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] != NULL
			    && compatible(lb[b->p + 1], b)) {
				mean = (b->energy + lb[b->p + 1]->energy) / 2.0;
				b->energy = mean;
				lb[b->p + 1]->energy = mean;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] != NULL
			    && compatible(lb[b->p - SX], b)) {
				mean =
				    (b->energy + lb[b->p - SX]->energy) / 2.0;
				b->energy = mean;
				lb[b->p - SX]->energy = mean;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] != NULL
			    && compatible(lb[b->p - 1], b)) {
				mean = (b->energy + lb[b->p - 1]->energy) / 2.0;
				b->energy = mean;
				lb[b->p - 1]->energy = mean;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] != NULL
			    && compatible(lb[b->p + SX], b)) {
				mean =
				    (b->energy + lb[b->p + SX]->energy) / 2.0;
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
	--b->age;
	if (b->pos > MEM_SIZE || b->ptr > MEM_SIZE || b->ptr < 0
	    || b->nl > MEM_SIZE)
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
		switch (b->dir) {
		case 0:
			if (b->p + 1 < SX * SY && lb[b->p + 1] != NULL
			    && compatible(lb[b->p + 1], b)) {
				b->memory[b->ptr] = 1;
			} else {
				b->memory[b->ptr] = 0;
			}
			break;
		case 1:
			if (b->p - SX >= 0 && lb[b->p - SX] != NULL
			    && compatible(lb[b->p - SX], b)) {
				b->memory[b->ptr] = 1;
			} else {
				b->memory[b->ptr] = 0;
			}
			break;
		case 2:
			if (b->p - 1 >= 0 && lb[b->p - 1] != NULL
			    && compatible(lb[b->p - 1], b)) {
				b->memory[b->ptr] = 1;
			} else {
				b->memory[b->ptr] = 0;
			}
			break;
		case 3:
			if (b->p + SX < SX * SY && lb[b->p + SX] != NULL
			    && compatible(lb[b->p + SX], b)) {
				b->memory[b->ptr] = 1;
			} else {
				b->memory[b->ptr] = 0;
			}
			break;
		}
		break;
	case 7:
		b->memory[b->ptr] = b->dir;
		break;
	case 8:
		run(b, lb);
		break;
	}
	//b->memory[b->ptr] = b->memory[b->ptr] % 9;
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
	int i, px, py, j, food = 4;
	unsigned short g[MEM_SIZE];
	bots = (struct bot *)malloc(sizeof(struct bot) * SX * SY);
	struct bot **lb = (struct bot **)malloc(sizeof(struct bot *) * SX * SY);
	for (i = 0; i < SX * SY; i++) {
		lb[i] = NULL;
	}
	for (j = 0; j < 5000; j++) {
		px = rand() % WIDTH;
		py = rand() % HEIGHT;
		for (i = 0; i < MEM_SIZE; i++) {
			g[i] = rand() % 9;
		}
		set_bot(&bots[last++], py * SX + px, 10000, g, lb, 0);
	}
	short get = 0, view = 0;
	char buf[20];
	while (!keypress) {
		++k;
		for (i = 0; i < last; i++) {
			compute(&bots[i], lb);
			switch (view) {
			case 0:
				setpixel(screen, bots[i].p % SX,
					 bots[i].p / SX % SY,
					 bots[i].r,
					 bots[i].g,
					 bots[i].b);
				break;
			case 1:
				setpixel(screen, bots[i].p % SX,
					 bots[i].p / SX % SY,
					 bots[i].energy / 10.0,
					 bots[i].energy / 100.0,
					 bots[i].energy / 1000.0);
				break;
			case 2:
				setpixel(screen, bots[i].p % SX,
					 bots[i].p / SX % SY,
					 bots[i].age / MAX_AGE * 255,
					 bots[i].age / MAX_AGE * 255,
					 bots[i].age / MAX_AGE * 255);
				break;
			case 3:
				setpixel(screen, bots[i].p % SX,
					 bots[i].p / SX % SY,
					 bots[i].generation / 2.0,
					 bots[i].generation / 2.0,
					 bots[i].generation / 2.0);
				break;
			case 4:
				if (bots[i].generation > 5)
					setpixel(screen, bots[i].p % SX,
						 bots[i].p / SX % SY,
						 bots[i].r,
						 bots[i].g,
						 bots[i].b);
				break;
			}
		}
		for (i = 0; i < SX * SY; i++) {
			lb[i] = NULL;
		}
		for (i = 0; i < last; i++) {
			if (bots[i].energy > 0 && bots[i].age > 0) {
				lb[bots[i].p] = &bots[i];
			} else {
				bots[i] = bots[--last];
			}
			if (get) {
				if (bots[i].energy > v
				    && bots[i].generation > 20) {
					b = i;
					v = bots[i].energy;
				}
			}
		}
		if (get) {
			for (i = 0; i < MEM_SIZE - 1; i++) {
				printf("%i, ", bots[b].gcode[i]);
			}
			printf("%i\n", bots[b].gcode[i]);
			v = 0;
			printf("##################\n");
			get = 0;
		}
		for (j = 0; j < food; j++) {
			px = rand() % WIDTH;
			py = rand() % HEIGHT;
			for (i = 0; i < MEM_SIZE; i++) {
				g[i] = rand() % 9;
			}
			if (lb[py * SX + px] == NULL)
				set_bot(&bots[last++], py * SX + px, 10000, g,
					lb, 0);
		}
		//if(k % 10 == 0) view = 0;
		//else view = 5;
		if (view != 5) {
			SDL_Flip(screen);
			sprintf(buf ,"%d", k / 10);
			//SDL_SaveBMP(screen, buf);
			//if(k % 1000 == 0)
			SDL_FillRect(screen, NULL, 0x000000);
			
		}
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
						printf
						    ("Filtered Genetic View\n");
						break;
					case 5:
						printf("Don't rendening\n");
						SDL_FillRect(screen, NULL,
							     0x000000);
						break;
					}
					break;
				case SDLK_UP:
					++food;
					printf("More Food -> %i\n", food);
					break;
				case SDLK_DOWN:
					--food;
					printf("Less Food -> %i\n", food);
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
        			if(event.button.button == 1)
               				if(lb[event.button.x + SX * event.button.y] != NULL){
						printf("GENETIC CODE: ");
						for (i = 0; i < MEM_SIZE - 1; i++) {
							printf("%i, ", lb[event.button.x + SX * event.button.y]->gcode[i]);
						}
						printf("%i\n", lb[event.button.x + SX * event.button.y]->gcode[i]);
					}
        			break;
			}
		}
	}
	return (0);
}
