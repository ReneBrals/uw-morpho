#include "chordSet.h"
#include <limits.h>
#include <stdlib.h>

chordSet initChordSet(){
	chordSet chords;
	chords.size = 0;
	chords.C = (chord*)malloc(1*sizeof(chord));
	chords.cap = 1;
	chords.minX = INT_MAX;
	chords.maxX = INT_MIN;
	chords.minY = INT_MAX;
	chords.maxY = INT_MIN;

	return chords;
}

void freeChordSet(chordSet SE){
	free(SE.C);
	SE.size = 0;
	SE.cap = 0;

	free(SE.R);
	SE.Lnum = 0;
}

void insertChordSet(chordSet* chords, chord c){
	if(chords->size == chords->cap){
		chords->C = (chord*)realloc(chords->C,chords->cap * 2 * sizeof(chord));
		chords->cap *= 2;
	}

	chords->C[chords->size].x = c.x;
	chords->C[chords->size].y = c.y;
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
}

int compChordLength(const void* p, const void* q){
	chord a,b;
	a = *((chord*)p);
	b = *((chord*)q);

	return (a.l > b.l) - (a.l < b.l);
}

int compChordX(const void* p, const void* q){
	chord a,b;
	a = *((chord*)p);
	b = *((chord*)q);

	return (a.x > b.x) - (a.x < b.x);
}

chordSet buildChordSet (image SE){
	chordSet chords = initChordSet();
	chord c;
	int chordStart,val;
	size_t i,j, x, y;

	int centerX = (SE.W-1)/2;
	int centerY = (SE.H-1)/2;

	for(y = 0; y < SE.H; y++){

		chordStart = -1;
		for(x = 0; x < SE.W; x++){
			if(SE.img[y][x] == 1 && chordStart == -1){
				chordStart = x;
			} else if (SE.img[y][x] == 0 && chordStart != -1){
				c.x = chordStart - centerX;
				c.y = y - centerY;
				c.l = x-chordStart;
				insertChordSet(&chords,c);
				chordStart = -1;
			}

		}

		if(chordStart != -1){
			c.x = chordStart - centerX;
			c.y = y - centerY;
			c.l = x-chordStart;
			insertChordSet(&chords,c);
		}
	}

	qsort(chords.C,chords.size,sizeof(chord),compChordLength);
	//printf("ok.\n");

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

	//qsort(chords.C,chords.size,sizeof(chord),compChordX);

	//printf("MinY: %d, MaxY: %d\n",chords.minY,chords.maxY);

	return chords;
}

void printChordSet(chordSet SE){
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
