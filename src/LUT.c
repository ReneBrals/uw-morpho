/* LUT.c
 * Functions pertaining to the computation of the lookup table
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <omp.h>
#include "safeMalloc.h"
#include "SIMD.h"
#include "LUT.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

void allocateLUT(LUT* Ty, chordSet SE){
	int r;
	size_t prePadX = 0, i;

	/*
	 * The LUT is pre-padded on X. This is needed for dilation, where the
	 * padding might contain valid information.
	 */
	if(SE.minX < 0){
		prePadX = 0-SE.minX;
	}
	Ty->padX = prePadX;

	Ty->arr = (unsigned char***)safeMalloc((Ty->maxR-Ty->minR+1) * sizeof(char**));
	for(r = 0; r < (Ty->maxR - Ty->minR + 1); r++){
		Ty->arr[r] = (unsigned char**)safeMalloc(Ty->I * sizeof(char*));
		for(i = 0; i < Ty->I ; i++){
			Ty->arr[r][i] = (unsigned char*)safeCalloc((Ty->X + prePadX), sizeof(char));

			/* Shifting the X index such that negative indices correspond to
			 * the pre-padding.
			 */
			Ty->arr[r][i] = &(Ty->arr[r][i][prePadX]);
		}

	}

	/*
	 * Shifting the R index so that it can be accessed from ymin <= r <= ymax
	 * without needing offsets.
	 */
	Ty->arr = &(Ty->arr[0 - Ty->minR]); //I'm sorry Valgrind...
}

void freeLUT(LUT table){
	int r;
	size_t i;

	// The R index was shifted, create a pointer to the original array
	unsigned char*** rp = &(table.arr[table.minR]);

	for(r=table.minR;r<=table.maxR;r++){
		for(i=0;i<table.I;i++){
			// The X index was also shifted, for padding purposes.
			free(table.arr[r][i] - table.padX);
		}
		free(table.arr[r]);
	}
	free(rp);
}

void circularSwapPointers(LUT Ty){
	unsigned char** Ty0;
	int r;

	/*
	 * Swap the pointers to r-indices in a circle. This is useful because
	 * Ty(r,i,x) = Ty-1(r+1,i,x) for r < ymax.
	 */
	if(Ty.maxR - Ty.minR > 0){
		Ty0 = Ty.arr[Ty.minR];

		for( r = Ty.minR; r< Ty.maxR; r++){
			Ty.arr[r] = Ty.arr[r+1];
		}

		r = Ty.maxR;
		Ty.arr[r] = Ty0;
	}
}

void computeMinRow(image f, LUT Ty, chordSet SE, int r, size_t y){
	size_t i, d;

	/*
	 * Algorithm II.1 in Urbach-Wilkinson paper.
	 * Computes lookup table for a single index of r.
	 */

	if(y+r >= 0 && y+r < f.H){
		memcpy(Ty.arr[r][0], f.img[y + r], Ty.X);
	} else {
		memset(Ty.arr[r][0], 0, Ty.X);
	}

	for(i=1;i<SE.Lnum;i++){
		d = SE.R[i] - SE.R[i - 1];
		simdMin(Ty.arr[r][i],
			Ty.arr[r][i - 1], Ty.arr[r][i - 1] + d,
			MAX((int)Ty.X - (int)d, 0));
	}
}

void updateMinLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num){
	/*
	 * When running in an OpenMP context, pointer swapping must be atomic.
	 */
	#pragma omp single
	{
		for(size_t i = 0; i < num; i++)
			circularSwapPointers(*Ty);
	}

	/*
	 * When running in an OpenMP context, the index to update is shifted by the
	 * thread ID.
	 */
	computeMinRow(f, *Ty, SE, Ty->maxR - tid, y);
}

LUT computeMinLUT(image f, chordSet SE, size_t y, size_t num){
	int r;

	LUT Ty;
	Ty.I = SE.Lnum;
	Ty.X = f.W;
	Ty.minR = SE.minY;
	Ty.maxR = SE.maxY + num - 1;
	allocateLUT(&Ty, SE);

	#pragma omp parallel for
	for(r=Ty.minR; r<=Ty.maxR; r++){
		computeMinRow(f, Ty, SE, r, y);
	}

	return Ty;
}

void computeMaxRow(image f, LUT Ty, chordSet SE, int r, size_t y){
	size_t i, d;

	/*
	 * Algorithm II.1 in Urbach-Wilkinson paper.
	 * Computes lookup table for a single index of r.
	 */

	if(y+r >= 0 && y+r < f.H){
		memcpy(Ty.arr[r][0], f.img[y + r], Ty.X);
	} else {
		memset(Ty.arr[r][0], 0, Ty.X);
	}

	for(i=1;i<SE.Lnum;i++){
		d = SE.R[i] - SE.R[i - 1];
		simdMax(Ty.arr[r][i] - Ty.padX, Ty.arr[r][i - 1] - Ty.padX,
			Ty.arr[r][i-1] + d - Ty.padX,
			Ty.X + Ty.padX - d);
		memcpy(Ty.arr[r][i] + Ty.X - d, Ty.arr[r][i - 1] + Ty.X - d, d);
	}
}

void updateMaxLUT(image f, LUT* Ty, chordSet SE, size_t y, size_t tid, size_t num){
	/*
	 * When running in an OpenMP context, pointer swapping must be atomic.
	 */
	#pragma omp single
	{
		for(size_t i = 0; i < num; i++)
			circularSwapPointers(*Ty);
	}

	/*
	 * When running in an OpenMP context, the index to update is shifted by the
	 * thread ID.
	 */
	computeMaxRow(f, *Ty, SE, Ty->maxR - tid, y);
}

LUT computeMaxLUT(image f, chordSet SE, size_t y, size_t num){
	int r;

	LUT Ty;
	Ty.I = SE.Lnum;
	Ty.X = f.W;
	Ty.minR = SE.minY;
	Ty.maxR = SE.maxY + num - 1;
	allocateLUT(&Ty, SE);

	#pragma omp parallel for
	for(r=Ty.minR; r<=Ty.maxR; r++){
		computeMaxRow(f, Ty, SE, r, y);
	}

	return Ty;
}

void printLUT(LUT Ty){
	int r;
	size_t i, x;

	for(r = Ty.minR; r <= Ty.maxR; r++){
		printf("r = %d:\n",r);
		for(i = 0; i < Ty.I; i++){
			printf("\t| %d", Ty.arr[r][i][0]);
			for(x = 1; x < Ty.X; x++){
				printf(" %d", Ty.arr[r][i][x]);
			}
			printf(" |\n");
		}
	}
}
