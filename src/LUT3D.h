#ifndef _LUT3
#define _LUT3

#include "chordSet3D.h"

#define MIN_LUT 0
#define MAX_LUT 1

typedef struct {
	unsigned char **** arr;
	int minR;
	int maxR;
	int minD;
	int maxD;
	int I;
	int X;
	int padX;
} LUT3D;

void printLUT3D(LUT3D Ty);
void updateMinLUT3D(image3D f, LUT3D* Ty, chordSet3D SE, int y, int z);
void updateMaxLUT3D(image3D f, LUT3D* Ty, chordSet3D SE, int y, int z);
void freeLUT3D(LUT3D table);
LUT3D computeMinLUT3D(image3D f, chordSet3D SE, int z);
LUT3D computeMaxLUT3D(image3D f, chordSet3D SE, int z);

#endif
