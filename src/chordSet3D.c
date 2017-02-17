/* chordSet3D.c
 * Functions related to decomposing a flat 3D image into a set of chords.
 * A chord is a run of non-zero pixels in a structuring element.
 */

#include <limits.h>
#include <stdlib.h>
#include "chordSet3D.h"
#include "safeMalloc.h"

chordSet3D initChordSet3D(){
	chordSet3D chords;
	chords.size = 0;
	chords.C = (chord3D*)malloc(1 * sizeof(chord3D));
	chords.cap = 1;
	chords.minX = INT_MAX;
	chords.maxX = INT_MIN;
	chords.minY = INT_MAX;
	chords.maxY = INT_MIN;
	chords.minZ = INT_MAX;
	chords.maxZ = INT_MIN;

	return chords;
}

void freeChordSet3D(chordSet3D SE){
	free(SE.C);
	SE.size = 0;
	SE.cap = 0;

	free(SE.R);
	SE.Lnum = 0;
}

void insertChordSet3D(chordSet3D* chords, chord3D c){
	// Checking if chord fits in dynamic array, resize if not.
	if(chords->size == chords->cap){
		chords->C = (chord3D*)safeRealloc(chords->C, chords->cap * 2 * sizeof(chord3D));
		chords->cap *= 2;
	}

	// Add the chord to the dynamic array.
	chords->C[chords->size].x = c.x;
	chords->C[chords->size].y = c.y;
	chords->C[chords->size].z = c.z;
	chords->C[chords->size++].l = c.l;

	// Update minimum/maximum x/y/z offsets of the chord set.
	if(c.x < chords->minX) chords->minX = c.x;
	if(c.x > chords->maxX) chords->maxX = c.x;

	if(c.y < chords->minY) chords->minY = c.y;
	if(c.y > chords->maxY) chords->maxY = c.y;

	if(c.z < chords->minZ) chords->minZ = c.z;
	if(c.z > chords->maxZ) chords->maxZ = c.z;
}

int compChord3DLength(const void* p, const void* q){
	chord3D a,b;
	a = *((chord3D*)p);
	b = *((chord3D*)q);

	return (a.l > b.l) - (a.l < b.l);
}

int compChord3DX(const void* p, const void* q){
	chord3D a,b;
	a = *((chord3D*)p);
	b = *((chord3D*)q);

	return (a.x > b.x) - (a.x < b.x);
}

int compChord3DY(const void* p, const void* q){
	chord3D a,b;
	a = *((chord3D*)p);
	b = *((chord3D*)q);

	return (a.y > b.y) - (a.y < b.y);
}

chordSet3D buildChordSet3D (image3D SE){
	chordSet3D chords = initChordSet3D();
	chord3D c;
	int chordStart, val;
	size_t i, chordLengthIndex, x, y, z;

	/*
	 * In erosion/dilation, the center of the image has S.E. offset (0,0).
	 * Otherwise, the resulting image would be shifted to the top-left.
	 */
	int centerX = (SE.W - 1) / 2;
	int centerY = (SE.H - 1) / 2;
	int centerZ = (SE.D - 1) / 2;

	/*
	 * Computing the set of chords C.
	 */
	for(z = 0; z < SE.D; z++){
		for(y = 0; y < SE.H; y++){
			chordStart = -1;
			for(x = 0; x < SE.W; x++){
				//A chord is a run of non-zero pixels.
				if(SE.img[z][y][x] != 0 && chordStart == -1){
					// Chord starts.
					chordStart = x;
				} else if (SE.img[z][y][x] == 0 && chordStart != -1){
					// Chord ends before end of line.
					c.x = chordStart - centerX;
					c.y = y - centerY;
					c.z = z - centerZ;
					c.l = x - chordStart;
					insertChordSet3D(&chords, c);
					chordStart = -1;
				}

			}

			if(chordStart != -1){
				// Chord ends at end of line.
				c.x = chordStart - centerX;
				c.y = y - centerY;
				c.z = z - centerZ;
				c.l = x - chordStart;
				insertChordSet3D(&chords, c);
			}
		}
	}

	/*
	 * Computing the array of chord lengths R(i).
	 * This is needed because the lookup table will contain a row for each
	 * length index i.
	 */
	qsort(chords.C, chords.size, sizeof(chord3D), compChord3DLength);

	chords.R = (int*)malloc(sizeof(int));
	chords.Lnum = 0;
	val=0;
	size_t rCap = 1;

	if(chords.size > 0){
		val = 1;
		if(chords.Lnum >= rCap){
			chords.R = (int*)realloc(chords.R,rCap * 2 * sizeof(int));
			rCap *=2;
		}
		chords.R[chords.Lnum++] = 1;
		val = 1;
	}


	for(i = 0;i<chords.size;i++){
		if(val != chords.C[i].l){
			while(2*val < chords.C[i].l && val != 0){
				/*
				 * Adding intermediary length such that consecutive lengths never more
				 * than double.
				 */
				if(chords.Lnum >= rCap){
					chords.R = (int*)realloc(chords.R,rCap * 2 * sizeof(int));
					rCap *=2;
				}
				chords.R[chords.Lnum++] = 2*val;
				val *= 2;
			}
			val = chords.C[i].l;

			if(chords.Lnum >= rCap){
				chords.R = (int*)realloc(chords.R,rCap * 2 * sizeof(int));
				rCap *=2;
			}
			chords.R[chords.Lnum++] = val;
		}
	}

	/*
	 * Setting the length indices of chords.
	 * These are needed so that the algorithm can, for each chord,
	 * access the lookup table at the correct length in constant time.
	 */
	chordLengthIndex = 0;
	for(i = 0; i < chords.size; i++){
		while(chords.R[chordLengthIndex] < chords.C[i].l){
			chordLengthIndex++;
		}
		chords.C[i].i = chordLengthIndex;
	}

	return chords;
}

void printChordSet3D(chordSet3D SE){
	size_t i;
	printf("Chords (%lu/%lu):\n\t[",SE.size,SE.cap);
	for(i=0;i<SE.size-1;i++){
		printf("<(%d,%d), %d>, ", SE.C[i].x, SE.C[i].y, SE.C[i].l);
	}
	printf("<(%d,%d), %d>]\n", SE.C[SE.size-1].x, SE.C[SE.size-1].y, SE.C[SE.size-1].l);

	printf("Lengths (%lu):\n\t[",SE.Lnum);

	for(i=0;i<SE.Lnum-1;i++){
		printf("%d, ", SE.R[i]);
	}
	printf("%d]\n", SE.R[SE.Lnum-1]);

}
