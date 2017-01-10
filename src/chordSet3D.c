#include "chordSet3D.h"
#include <limits.h>
#include <stdlib.h>

chordSet3D initChordSet3D(){
	chordSet3D chords;
	chords.size = 0;
	chords.C = (chord3D*)malloc(1*sizeof(chord3D));
	chords.cap = 1;
	chords.minX = INT_MAX;
	chords.maxX = INT_MIN;
	chords.minY = INT_MAX;
	chords.maxY = INT_MIN;
	chords.minZ = INT_MAX;
	chords.maxZ = INT_MAX;

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
	if(chords->size == chords->cap){
		chords->C = (chord3D*)realloc(chords->C,chords->cap * 2 * sizeof(chord3D));
		chords->cap *= 2;
	}

	chords->C[chords->size].x = c.x;
	chords->C[chords->size].y = c.y;
	chords->C[chords->size].z = c.z;
	chords->C[chords->size++].l = c.l;


	if(c.x < chords->minX){
		chords->minX = c.x;
	}

	if(c.x > chords->maxX){
		chords->maxX = c.x;
	}

	if(c.y < chords->minY){
		chords->minY = c.y;
	}

	if(c.y > chords->maxY){
		chords->maxY = c.y;
	}

	if(c.y < chords->minZ){
		chords->minY = c.z;
	}

	if(c.y > chords->maxZ){
		chords->maxY = c.z;
	}
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

chordSet3D buildChordSet3D (image3D SE){
	chordSet3D chords = initChordSet3D();
	chord3D c;
	int chordStart,val;
	size_t i,j, x, y, z;

	int centerX = (SE.W-1)/2;
	int centerY = (SE.H-1)/2;
	int centerZ = (SE.D-1)/2;

	for(z = 0; z < SE.D; z++){
		for(y = 0; y < SE.H; y++){
			chordStart = -1;
			for(x = 0; x < SE.W; x++){
				if(SE.img[z][y][x] == 1 && chordStart == -1){
					chordStart = x;
				} else if (SE.img[z][y][x] == 0 && chordStart != -1){
					c.x = chordStart - centerX;
					c.y = y - centerY;
					c.z = z - centerZ;
					c.l = x-chordStart;
					insertChordSet3D(&chords,c);
					chordStart = -1;
				}

			}

			if(chordStart != -1){
				c.x = chordStart - centerX;
				c.y = y - centerY;
				c.z = z - centerZ;
				c.l = x-chordStart;
				insertChordSet3D(&chords,c);
			}
		}
	}

	qsort(chords.C,chords.size,sizeof(chord3D),compChord3DLength);

	chords.R = (int*)malloc(sizeof(int));
	chords.Lnum = 0;
	val=0;
	size_t rCap = 1;

	if(chords.size >0){
		val = 1;
		if(chords.Lnum >= rCap){
			chords.R = (int*)realloc(chords.R,rCap*2*sizeof(int));
			rCap *=2;
		}
		chords.R[chords.Lnum++] = 1;
		val = 1;
	}

	for(i=0;i<chords.size;i++){
		if(val != chords.C[i].l){
			while(2*val < chords.C[i].l && val != 0){
				//Add intermediary length
				if(chords.Lnum >= rCap){
					chords.R = (int*)realloc(chords.R,rCap*2*sizeof(int));
					rCap *=2;
				}
				chords.R[chords.Lnum++] = 2*val;
				val *= 2;
			}
			val = chords.C[i].l;

			if(chords.Lnum >= rCap){
				chords.R = (int*)realloc(chords.R,rCap*2*sizeof(int));
				rCap *=2;
			}
			chords.R[chords.Lnum++] = val;
		}
	}
	j=0;
	for(i=0;i<chords.size;i++){
		while(chords.R[j]<chords.C[i].l){
			j++;
		}
		chords.C[i].i = j;
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
