/* LUT.c
 * Defines the lookup table required by the Urbach-Wilkinson algorithm.
 */
 
#ifndef _LUT
#define _LUT

#include "chordSet.h"

typedef struct {
	unsigned char *** arr;
	int minR;
	int maxR;
	size_t I;
	size_t X;
	int padX;
} LUT;

void printLUT(LUT Ty);
void updateMinLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num);
void updateMaxLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num);
void freeLUT(LUT table);
LUT computeMinLUT(image f, chordSet SE, size_t y, size_t num);
LUT computeMaxLUT(image f, chordSet SE, size_t y, size_t num);

#endif
