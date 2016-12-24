#ifndef _LUT
#define _LUT

#include "chordSet.h"

typedef struct {
	unsigned char *** arr;
	int minR;
	int maxR;
	int I;
	int X;
	int padX;
} LUT;

void printLUT(LUT Ty);
void updateLUT(image f, LUT* Ty, chordSet SE, int y);
void freeLUT(LUT table);
LUT computeLUT(image f, chordSet SE);

#endif
