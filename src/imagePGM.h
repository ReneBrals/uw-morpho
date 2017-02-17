/* imagePGM.h
 * Image file I/O.
 */

#ifndef _IMGPGM
#define _IMGPGM
#include <stdio.h>

typedef struct {
	unsigned char* bytes;
	unsigned char** img;
	size_t W;
	size_t H;
	size_t range;
} image;

void allocateImage(image* imp);
void freeImage(image imp);
void imageDiffInPlace(image f, image g);
image readPGM(const char* filename);
image readImage(const char* filename);
void writePGM(const char* filename, image im);
image disk(int r);
void printPBM(image im);
image initImage(size_t w, size_t h, size_t range);
image initImagePreallocated(unsigned char* x, size_t w, size_t h, size_t range);

image h(int d);
image checkerboard(int d);
image rectangle(int d);
image noise(int d);

#endif
