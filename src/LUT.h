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
void updateMinLUT(image f, LUT* Ty, chordSet SE, int y);
void updateMaxLUT(image f, LUT* Ty, chordSet SE, int y);
void freeLUT(LUT table);
LUT computeMinLUT(image f, chordSet SE);
LUT computeMaxLUT(image f, chordSet SE);

#endif
