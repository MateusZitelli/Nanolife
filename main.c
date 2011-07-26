#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
//#include "SDL/SDL_gfxPrimitives.h"
#define SX 200
#define SY 200
#define MEM_SIZE 128
#define WIDTH 200
#define HEIGHT 200
#define BPP 4
#define DEPTH 32

int last;
struct bot * bots;
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
	unsigned short ptr;
	unsigned short pos;
	unsigned short dir;
	int color;
	short age;
};

void set_bot(struct bot *b, int p, float e, unsigned short *g, struct bot ** lb){
	short i;
	int cr = 0,cg = 0,cb = 0;
	b->p = p;
	if(e < 0) e = 0;
	b->energy = e;
	for(i = 0; i < MEM_SIZE; i++){
		b->gcode[i] = g[i];
		b->memory[i] =  0;
	}
	b->nl = 0;
	b->ptr = 0;
	b->pos = 0;
	b->dir = rand() % 4;
	for(i = 0; i < MEM_SIZE / 3; i++){
		cr += g[i];
	}
	for(i = MEM_SIZE / 3; i < MEM_SIZE / 3 * 2; i++){
		cg += g[i];
	}
	for(i = MEM_SIZE / 3 * 2; i < MEM_SIZE; i++){
		cb += g[i];
	}
	cr = (int)((float)cr / (MEM_SIZE / 3) * 256) % 256;
	cg = (int)((float)cg / (MEM_SIZE / 3) * 256) % 256;
	cb = (int)((float)cb / (MEM_SIZE / 3) * 256) % 256;
	b->color = cr << 16 | cg << 8 | cb; 
	b->age = 1000;
}

void run(struct bot *b, struct bot **lb){
	//printf("%i -> %i\n", b->ptr, b->memory[b->ptr]);
	unsigned short ngcode[MEM_SIZE], i;
	int mean;
	switch(b->memory[b->ptr]){
		case 1:
			b->dir = (b->dir + 1) % 4;
			break;
		case 2:
			b->dir = (b->dir - 1) % 4;
			break;
		case 3:
			//b->energy += 10;
			switch(b->dir){
				case 2:
					if(b->p - 1 < SX * SY && b->p - 1 > 0 && lb[b->p - 1] == NULL)
						b->p++;
					break;
				case 3:
					if(b->p + SX < SX * SY && b->p + SX > 0 && lb[(b->p + SX) % (SY * SX)] == NULL)
						b->p = (b->p + SX) % (SY * SX);
					break;
				case 0:
					if(b->p + 1 < SX * SY && b->p + 1 > 0 && lb[b->p + 1] == NULL)
						b->p--;
					break;
				case 1:
					if(b->p - SX < SX * SY && b->p - SX > 0 && lb[(b->p - SX) % (SY * SX)] == NULL)
						b->p = (b->p + SX) % (SY * SX);
					break;
			}
			break;
		case 4:
			//b->energy += 10;
			switch(b->dir){
				case 0:
					if(b->p - 1 < SX * SY && b->p - 1 >= 0 && lb[b->p - 1] != NULL){
						lb[b->p - 1]->energy -= b->energy / 2.0; 
						b->energy *= 1.5;
					}else
						b->p++;
					break;
				case 1:
					if(b->p + SX < SX * SY && b->p + SX >= 0 && lb[(b->p + SX) % (SY * SX)] != NULL){
						lb[(b->p + SX) % (SY * SX)]->energy -= b->energy / 2.0; 
						b->energy *= 1.5;
					}else
						b->p = (b->p + SX) % (SY * SX);
					break;
				case 2:
					if(b->p + 1 < SX * SY && b->p + 1 >= 0 && lb[b->p + 1] != NULL){
						lb[b->p + 1]->energy -= b->energy / 2.0; 
						b->energy *= 1.5;
					}else
						b->p--;
					break;
				case 3:
					if(b->p - SX < SX * SY && b->p - SX >= 0 && lb[(b->p - SX) % (SY * SX)] != NULL){
						lb[(b->p - SX) % (SY * SX)]->energy -= b->energy / 2.0;
						b->energy *= 1.5;
					}else
						b->p = (b->p + SX) % (SY * SX);
					break;
			}
			break;
			break;
		case 5:
			//b->age += 10000;
			//b->energy += 1000;
			for(i = 0; i < MEM_SIZE; i++){
				ngcode[i] = b->gcode[i];
			}
			if(rand() % 1000 > 900)
				ngcode[rand() % MEM_SIZE] = rand() % 8;
			switch(b->dir){
				case 0:
					if(b->p + 1 < SX * SY && b->p + 1 >= 0 && lb[b->p + 1] == NULL){
						lb[b->p + 1] = &bots[last];
						set_bot(&bots[last++], b->p + 1, b->energy / 2.0, ngcode, lb);
						b->energy /= 2.0;
					}
					break;
				case 1:
					if(b->p - SX < SX * SY && b->p - SX >= 0 && lb[(b->p - SX) % (SY * SX)] == NULL){
						lb[(b->p - SX) % (SY * SX)] = &bots[last];
						set_bot(&bots[last++], (b->p - SX) % (SY * SX), b->energy / 2.0, ngcode, lb);
						b->energy /= 2.0;
					}
					break;
				case 2:
					if(b->p - 1 < SX * SY && b->p - 1 >= 0 && lb[b->p - 1] == NULL){
						lb[b->p - 1] = &bots[last];
						set_bot(&bots[last++], b->p - 1, b->energy / 2.0, ngcode, lb);
						b->energy /= 2.0;
					}
					break;
				case 3:
					if(b->p + SX < SX * SY && b->p + SX >= 0 && lb[(b->p + SX) % (SY * SX)] == NULL){
						lb[(b->p + SX) % (SY * SX)] = &bots[last];
						set_bot(&bots[last++], (b->p + SX) % (SY * SX), b->energy / 2.0, ngcode, lb);
						b->energy /= 2.0;
					}
					break;
			}
			break;
		case 6:
			switch(b->dir){
				case 0:
					if(b->p + 1 < SX * SY && b->p + 1 >= 0 && lb[b->p + 1] != NULL){
						b->energy += 100;
						lb[b->p + 1]->energy -= 100; 
					}
					break;
				case 1:
					if(b->p - SX < SX * SY && b->p - SX >= 0 && lb[(b->p - SX) % (SY * SX)] != NULL){
						b->energy += 100;
						lb[(b->p - SX) % (SY * SX)]->energy -= 100; 
					}
					break;
				case 2:
					if(b->p - 1 < SX * SY && b->p - 1 >= 0 && lb[b->p - 1] != NULL){
						b->energy += 100;
						lb[b->p - 1]->energy -= 100; 
					}
					break;
				case 3:
					if(b->p + SX < SX * SY && b->p + SX >= 0 && lb[(b->p + SX) % (SY * SX)] != NULL){
						b->energy += 100;
						lb[(b->p + SX) % (SY * SX)]->energy -= 100; 
					}
					break;
				break;
			case 7:
				switch(b->dir){
					case 0:
						if(b->p + 1 < SX * SY && b->p + 1 >= 0 && lb[b->p + 1] != NULL){
							mean = (b->energy + lb[b->p + 1]->energy) / 2.0;
							lb[b->p + 1]->energy = mean; 
							b->energy = mean;
						}
						break;
					case 1:
						if(b->p - SX < SX * SY && b->p - SX >= 0 && lb[(b->p - SX) % (SY * SX)] != NULL){
							mean = (b->energy + lb[(b->p - SX) % (SY * SX)]->energy) / 2.0;
							lb[b->p + 1]->energy = mean; 
							b->energy = mean;
						}
						break;
					case 2:
						if(b->p - 1 < SX * SY && b->p - 1 >= 0 && lb[b->p - 1] != NULL){
							mean = (b->energy + lb[b->p - 1]->energy) / 2.0;
							lb[b->p + 1]->energy = mean; 
							b->energy = mean;
						}
						break;
					case 3:
						if(b->p + SX < SX * SY && b->p + SX >= 0 && lb[(b->p + SX) % (SY * SX)] != NULL){
							mean = (b->energy + lb[(b->p + SX) % (SY * SX)]->energy) / 2.0;
							lb[b->p + 1]->energy = mean; 
							b->energy = mean;
						}
						break;
				}
				break;
			}
			break;
	}
}

void compute(struct bot *b, struct bot ** lb){
	//printf("1 %i -> %i\n", b->pos, b->gcode[b->pos]);
	switch(b->gcode[b->pos++]){
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
			if(b->gcode[b->ptr])
				b->loops[b->nl] = b->pos;
				b->loops_ptr[b->nl++] = b->ptr;
			break;
		case 5:
			if(b->nl && b->memory[b->loops_ptr[b->nl - 1]]){
				b->pos = b->loops[b->nl - 1] + 1;
			}else{
				--b->nl;
			}
			break;
		case 6:
			switch(b->dir){
				case 0:
					if(b->p + 1 < SX * SY && b->p + 1 >= 0 && lb[b->p + 1] == NULL){
						b->memory[b->ptr] = 0;
					}else{
						b->memory[b->ptr] = 1;
					}
					break;
				case 1:
					if(b->p - SX < SX * SY && b->p - SX >= 0 && lb[(b->p - SX) % (SY * SX)] == NULL){
						b->memory[b->ptr] = 0;
					}else{
						b->memory[b->ptr] = 1;
					}
					break;
				case 2:
					if(b->p - 1 < SX * SY && b->p - 1 >= 0 && lb[b->p - 1] == NULL){
						b->memory[b->ptr] = 0;
					}else{
						b->memory[b->ptr] = 1;
					}
					break;
				case 3:
					if(b->p + SX < SX * SY && b->p + SX >= 0 && lb[(b->p + SX) % (SY * SX)] == NULL){
						b->memory[b->ptr] = 0;
					}else{
						b->memory[b->ptr] = 1;
					}
					break;
			}
			break;
		case 7:
			run(b, lb);
			break;
	}
	b->pos %= MEM_SIZE;
	b->ptr %= MEM_SIZE;
	--b->age;
	--b->energy;
	b->memory[b->ptr] = b->memory[b->ptr] % 8;
}

void setpixel(SDL_Surface * screen, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
	Uint32 *pixmem32;
	Uint32 colour;

	colour = SDL_MapRGB(screen->format, r, g, b);

	pixmem32 = (Uint32 *) screen->pixels + screen->w * y + x;
	*pixmem32 = colour;
}

int main(void){
	srand(time(0));
	SDL_Surface *screen;
	SDL_Event event;

	int keypress = 0;
	int h = 0, k = 0;
	long long b = 0, v = 0;
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	if (!(screen = SDL_SetVideoMode(WIDTH, HEIGHT, DEPTH, SDL_HWSURFACE))) {
		SDL_Quit();
		return 1;
	}
	int i, px, py, j;
	unsigned short g[MEM_SIZE];
	bots = (struct bot *) malloc(sizeof(struct bot) * SX * SY);
	struct bot ** lb = (struct bot **) malloc(sizeof(struct bot *) * SX * SY);
	for(i = 0; i < SX * SY; i++){
		lb[i] = NULL;
	}
	for(j = 0; j < 0; j++){
		px = rand() % WIDTH;
		py = rand() % HEIGHT;
		for(i = 0; i < MEM_SIZE; i++){
			g[i] = rand() % 8;
		}
		set_bot(&bots[last++], py * SX + px, 100, g, lb);
	}
	short get = 0;
	while(!keypress){
		for(i = 0; i < last; i++){
			compute(&bots[i], lb);
			setpixel(screen, bots[i].p % SX, bots[i].p / SX % SY, bots[i].color >> 16, (bots[i].color >> 8) & 0xFF, bots[i].color & 0xFF);
			//printf("%i, %i\n", bots[i].p % SX, bots[i].p / SX);
		}
		for(i = 0; i < last; i++){
			if(get){
				if(bots[i].energy > v){
					b = i;
					v = bots[i].energy;
				}
			}
			v += bots[i].energy;
			if(bots[i].energy <= 0 || bots[i].age <= 0){
				if(bots[i].p < 0) bots[i].p = 0;
				else if(bots[i].p >= SX * SY) bots[i].p = SX * SY - 1;
				lb[bots[i].p] = NULL;
				bots[i] = bots[--last];
			}
		}
		if(get){
			for(i = 0; i < MEM_SIZE; i++){
				printf("%i\n", bots[b].gcode[i]);
			}
			v = 0;
			printf("##################\n");
			get = 0;
		}
		for(j = 0; j < 20; j++){
			px = rand() % WIDTH;
			py = rand() % HEIGHT;
			for(i = 0; i < MEM_SIZE; i++){
				g[i] = rand() % 8;
			}
			if(lb[py * SX + px] == NULL)
				set_bot(&bots[last++], py * SX + px, 100, g, lb);
		}
		if(k % 1== 0){
			SDL_Flip(screen);
			//if(k % 1000 == 0)
				SDL_FillRect(screen, NULL, 0x000000);
		}
		k++;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				keypress = 1;
				break;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_g:
						get = 1;
						break;
				}
				break;
			}
		}
	}
	return(0);
}

