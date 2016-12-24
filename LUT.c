#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include "simdMin.h"
#include <time.h>

#include "LUT.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

struct timespec sp, ep;

void printLUT(LUT Ty){
	int r, i, x;

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

void updateLUT(image f, LUT* Ty, chordSet SE, int y){
	int r;
	size_t i;
	int d;

	//Ty(i,x,r) = Ty-1(i,x,r+1) 	if ymin <= r < ymax
	//Done by swapping pointers in a circle.

	if(Ty->maxR - Ty->minR > 0){
		unsigned char** Ty0 = Ty->arr[Ty->minR];

		for( r = Ty->minR; r< Ty->maxR; r++){
			Ty->arr[r] = Ty->arr[r+1];
		}

		r = Ty->maxR;
		Ty->arr[r] = Ty0;
	}

	//Ty(i,x,r) = min[a=0 -> R(i) - 1] f(x+a,y+r)	if r = ymax

	r = Ty->maxR;

	memcpy(Ty->arr[r][0], f.img[y+r], Ty->X);

	for(i=1;i<SE.Lnum;i++){
		d = SE.R[i] - SE.R[i-1];
		simdMin(Ty->arr[r][i], Ty->arr[r][i-1], Ty->arr[r][i-1] + d, Ty->X);
	}
}

void freeLUT(LUT table){
	int r,i;

	//The R index was shifted
	unsigned char*** rp = &(table.arr[table.minR]);

	for(r=table.minR;r<=table.maxR;r++){
		for(i=0;i<table.I;i++){
			//The X index was shifted for padding
			free(table.arr[r][i] - table.padX);
		}
		free(table.arr[r]);
	}
	free(rp);
}

void allocateLUT(LUT* Ty, chordSet SE){
	int r,i;
	int prePadX=0, postPadX=0;

	if(SE.minX < 0){
		prePadX = 0-SE.minX;
	}

	Ty->padX = prePadX;

	if(SE.maxX > 0 || SE.R[SE.Lnum-1] > 0){

		postPadX = MAX( SE.maxX, SE.R[SE.Lnum-1]);
	}

	Ty->arr = (unsigned char***)malloc((Ty->maxR-Ty->minR+1) * sizeof(char**));
	for(r = 0; r < (Ty->maxR-Ty->minR+1); r++){
		Ty->arr[r] = (unsigned char**)malloc(Ty->I * sizeof(char*));
		for(i = 0; i < Ty->I ;i++){
			Ty->arr[r][i] = (unsigned char*)calloc((Ty->X + prePadX + postPadX),sizeof(char));

			//Shifting the X index such that negative indexes correspond to the prepadding.
			Ty->arr[r][i] = &(Ty->arr[r][i][prePadX]);
		}

	}

	// Shifting the R index so that it can be accessed from ymin <= r <= ymax
	// without needing offsets
	Ty->arr = &(Ty->arr[0 - Ty->minR]); //I'm sorry Valgrind...
}

LUT computeLUT(image f, chordSet SE){
	int r,x;
	size_t i;
	int d;

	LUT Ty;
	Ty.I = SE.Lnum;
	Ty.X = f.W;
	Ty.minR = SE.minY;
	Ty.maxR = SE.maxY;
	allocateLUT(&Ty, SE);

	for(r=Ty.minR; r<=Ty.maxR; r++){

		for(x = 0; x < f.W; x++){
			Ty.arr[r][0][x] = f.img[r][x];
		}
		for(i=1;i<SE.Lnum;i++){
			d = SE.R[i] - SE.R[i-1];

			simdMin(Ty.arr[r][i], Ty.arr[r][i-1], &(Ty.arr[r][i-1][d]),Ty.X);
		}
	}

	return Ty;
}
