#ifndef _CSET3
#define _CSET3

#include <stdlib.h>
#include "imageP3M.h"

typedef struct {
	int x;
	int y;
	int z;
	int l;

	int i;
} chord3D;

typedef struct {
	chord3D* C;
	size_t size;
	size_t cap;

	int* R;
	size_t Lnum;

	int minX;
	int maxX;
	int minY;
	int maxY;
	int minZ;
	int maxZ;
} chordSet3D;

chordSet3D initChordSet3D();
void insertChordSet3D(chordSet3D* chords, chord3D c);
chordSet3D buildChordSet3D (image3D SE);
void printChordSet3D(chordSet3D SE);
void freeChordSet3D(chordSet3D SE);

#endif
