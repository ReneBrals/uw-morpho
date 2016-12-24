#ifndef _IMGPGM
#define _IMGPGM
#include <stdio.h>

typedef struct {
	unsigned char* bytes;
	unsigned char** img;
	int W;
	int H;
	int range; //Usually 255, not always though!
	int padX;
	int padY;
} image;

void allocateImage(image* imp, int x, int preY, int postY);
void freeImage(image imp);
image readPGM(char* filename, int padX, int prePadY, int postPadY);
void writePGM(char* filename, image im);
image disk(int r);
void printPBM(image im);

#endif
