#ifndef _IMGPGM3
#define _IMGPGM3
#include <stdio.h>

typedef struct {
	unsigned char** bytes;
	unsigned char*** img;
	size_t W;
	size_t H;
	size_t D;
	size_t range; //Usually 255, not always though!
	size_t padX;
	size_t padY;
	size_t padZ;
} image3D;

void freeImage3D(image3D imp);
void image3DDiffInPlace(image3D f, image3D g);
image3D sphere(int r);
void printP3BM(image3D im);

image3D readP3M(const char* filename, int padX, int prePadY, int postPadY, int prepadZ, int postPadZ);
void writeP3M(const char* filename, image3D im3);
void allocateImage3D(image3D* im3p, int x, int preY, int postY, int preZ, int postZ);

#endif
