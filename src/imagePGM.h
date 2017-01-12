#ifndef _IMGPGM
#define _IMGPGM
#include <stdio.h>

typedef struct {
	unsigned char* bytes;
	unsigned char** img;
	size_t W;
	size_t H;
	size_t range; //Usually 255, not always though!
	size_t padY;
} image;

void allocateImage(image* imp, int preY, int postY);
void freeImage(image imp);
void imageDiffInPlace(image f, image g);
image readPGM(const char* filename, int prePadY, int postPadY);
void writePGM(const char* filename, image im);
image disk(int r);
void printPBM(image im);

#endif
