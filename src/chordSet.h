/* chordSet.h
 * Functions related to decomposing a flat image into a set of chords.
 * A chord is a run of non-zero pixels in a structuring element.
 */
 
#ifndef _CSET
#define _CSET

#include <stdlib.h>
#include "imagePGM.h"

typedef struct {
	int x;
	int y;
	int l;

	int i;
} chord;

typedef struct {
	chord* C;
	size_t size;
	size_t cap;

	int* R;
	size_t Lnum;

	int minX;
	int maxX;
	int minY;
	int maxY;
} chordSet;

chordSet initChordSet();
void insertChordSet(chordSet* chords, chord c);
chordSet buildChordSet (image SE);
void printChordSet(chordSet SE);
void freeChordSet(chordSet SE);

#endif
