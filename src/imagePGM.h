#ifndef _IMGPGM
#define _IMGPGM
#include <stdio.h>

typedef struct {
	unsigned char* bytes;
	unsigned char** img;
	size_t W;
	size_t H;
	size_t range; //Usually 255, not always though!
} image;

void allocateImage(image* imp);
void freeImage(image imp);
void imageDiffInPlace(image f, image g);
image readPGM(const char* filename);
void writePGM(const char* filename, image im);
image disk(int r);
void printPBM(image im);

#endif
