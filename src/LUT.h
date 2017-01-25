#ifndef _LUT
#define _LUT

#include "chordSet.h"

#define MIN_LUT 0
#define MAX_LUT 1

typedef struct {
	unsigned char *** arr;
	int minR;
	int maxR;
	int I;
	int X;
	int padX;
} LUT;

void printLUT(LUT Ty);
void updateMinLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num);
void updateMaxLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num);
void freeLUT(LUT table);
LUT computeMinLUT(image f, chordSet SE, size_t y);
LUT computeMaxLUT(image f, chordSet SE, size_t y);

#endif
