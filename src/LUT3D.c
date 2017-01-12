#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include "SIMD.h"
#include "LUT3D.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

//TODO: find a good way to print this.
void printLUT3D(LUT3D Ty){
	int d, r, i, x;
	for(d = Ty.minD; d <= Ty.maxD; d++){
		for(r = Ty.minR; r <= Ty.maxR; r++){
			printf("r = %d:\n",r);
			for(i = 0; i < Ty.I; i++){
				printf("\t| %d", Ty.arr[d][r][i][0]);
				for(x = 1; x < Ty.X; x++){
					printf(" %d", Ty.arr[d][r][i][x]);
				}
				printf(" |\n");
			}
		}
	}
}

void allocateLUT3D(LUT3D* Ty, chordSet3D SE){
	int r,d,i;
	int prePadX = 0, postPadX = 0;

	if(SE.minX < 0){
		prePadX = 0-SE.minX;
	}

	Ty->padX = prePadX;

	if(SE.maxX > 0 || SE.R[SE.Lnum-1] > 0){

		postPadX = MAX( SE.maxX, SE.R[SE.Lnum-1]);
	}

	for(r = 0; r < (Ty->maxR-Ty->minR+1); r++){
		Ty->arr[r] = (unsigned char***)malloc((Ty->maxD - Ty->minD + 1) * sizeof(char*));
		for(d = 0; d < (Ty->maxD - Ty->minD + 1); d++){
			Ty->arr[r][d] = (unsigned char**)malloc(Ty->I * sizeof(char*));
			for(i = 0; i < Ty->I ;i++){
				Ty->arr[r][d][i] = (unsigned char*)calloc((Ty->X + prePadX + postPadX),sizeof(char));

				//Shifting the X index such that negative indexes correspond to the prepadding.
				Ty->arr[r][d][i] = &(Ty->arr[r][d][i][prePadX]);
			}
		}
	}

	// Shifting the D indices so that it can be accessed from ymin <= r <= ymax
	// without needing to introduce padding and offset values in the computation code.
	for(r = 0; r < (Ty->maxR-Ty->minR+1); r++){

		Ty->arr[r] = &(Ty->arr[r][0 - Ty->minD]); //Access negative array indices with this weird old trick! Valgrind hates him!
	}

	// Same for R in the range ymin <= r <= ymax
	Ty->arr = &(Ty->arr[0 - Ty->minR]);
}



void freeLUT3D(LUT3D table){
	int r,d,i;

	//The R index was shifted
	unsigned char**** rp = &(table.arr[table.minR]);
	unsigned char*** dp;

	for(r = table.minR; r <= table.maxR; r++){
		dp = &(table.arr[r][table.minD]);

		for(d = table.minD; d <= table.maxD; d++){
			for(i=0;i<table.I;i++){
				//The X index was shifted for padding
				free(table.arr[r][d][i] - table.padX);
			}
			free(table.arr[r][d]);
		}

		free(dp);
	}
	free(rp);
}

void circularSwapPointers3D(LUT3D Ty){
	unsigned char*** Ty0;
	int r;

	if(Ty.maxR - Ty.minR > 0){
		Ty0 = Ty.arr[Ty.minR];

		for( r = Ty.minR; r< Ty.maxR; r++){
			Ty.arr[r] = Ty.arr[r+1];
		}

		r = Ty.maxR;
		Ty.arr[r] = Ty0;
	}
}

void computeMinRow3D(image3D f, LUT3D Ty, chordSet3D SE, int r, int d, size_t y, size_t z){
	size_t i, dd;

	if(y+r >= 0 && y+r < f.H && z+d >= 0 && z+d < f.D){
		memcpy(Ty.arr[r][d][0], f.img[y+r][z+d], Ty.X);
	} else {
		memset(Ty.arr[r][d][0], 0, Ty.X);
	}

	for(i=1;i<SE.Lnum;i++){
		dd = SE.R[i] - SE.R[i-1];
		simdMin(Ty.arr[r][d][i], Ty.arr[r][d][i-1], Ty.arr[r][d][i-1] + dd, Ty.X);
	}
}

void computeMaxRow3D(image3D f, LUT3D Ty, chordSet3D SE, int r, int d, size_t y, size_t z){
	size_t i, dd;

	if(y+r >= 0 && y+r < f.H && z+d >= 0 && z+d < f.D){
		memcpy(Ty.arr[r][d][0], f.img[y+r][z+d], Ty.X);
	} else {
		memset(Ty.arr[r][d][0], 0, Ty.X);
	}

	for(i=1;i<SE.Lnum;i++){
		dd = SE.R[i] - SE.R[i-1];
		simdMax(Ty.arr[r][d][i] - Ty.padX, Ty.arr[r][d][i-1] - Ty.padX, Ty.arr[r][d][i-1] + dd - Ty.padX, Ty.X + Ty.padX);
	}
}

void updateMinLUT3D(image3D f, LUT3D* Ty, chordSet3D SE, int y, int z){
	int d;

	circularSwapPointers3D(*Ty);
	for(d=Ty->minD; d<=Ty->maxD; d++){
		computeMinRow3D(f, *Ty, SE, Ty->maxR, d, y, z);
	}
}

void updateMaxLUT3D(image3D f, LUT3D* Ty, chordSet3D SE, int y, int z){
	int d;

	circularSwapPointers3D(*Ty);
	for(d = Ty->minD; d <= Ty->maxD; d++){
		computeMaxRow3D(f, *Ty, SE, Ty->maxR, d, y, z);
	}
}

//calculates T<0,z>(d,r,i,x)
LUT3D computeMinLUT3D(image3D f, chordSet3D SE, int z){
	int r,d;

	LUT3D Ty;
	Ty.I = SE.Lnum;
	Ty.X = f.W;
	Ty.minR = SE.minY;
	Ty.maxR = SE.maxY;
	Ty.minD = SE.minZ;
	Ty.maxD = SE.maxZ;

	allocateLUT3D(&Ty, SE);

	for(d=Ty.minD; d<=Ty.maxD; d++){
		for(r=Ty.minR; r<=Ty.maxR; r++){
			computeMinRow3D(f, Ty, SE, r, d, 0, z);
		}
	}

	return Ty;
}

//calculates T<0,z>(d,r,i,x)
LUT3D computeMaxLUT3D(image3D f, chordSet3D SE, int z){
	int r,d;

	LUT3D Ty;
	Ty.I = SE.Lnum;
	Ty.X = f.W;
	Ty.minR = SE.minY;
	Ty.maxR = SE.maxY;
	allocateLUT3D(&Ty, SE);

	for(d=Ty.minD; d<=Ty.maxD; d++){
		for(r=Ty.minR; r<=Ty.maxR; r++){
			computeMaxRow3D(f, Ty, SE, r, d, 0, z);
		}
	}

	return Ty;
}
