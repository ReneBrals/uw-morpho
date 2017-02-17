/* LUTSplitting.h
 * Defines the lookup table required by the Urbach-Wilkinson algorithm
 * (alternate method of parallelization).
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
void updateMinLUT(image f, LUT* Ty, chordSet SE, size_t y);
void updateMaxLUT(image f, LUT* Ty, chordSet SE, size_t y);
void freeLUT(LUT table);
LUT computeMinLUT(image f, chordSet SE, size_t y);
LUT computeMaxLUT(image f, chordSet SE, size_t y);

#endif
