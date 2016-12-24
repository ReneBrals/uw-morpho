#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include "chordSet.h"
#include "imagePGM.h"
#include "LUT.h"
#include <omp.h>
#include <string.h>
#include "simdMin.h"
#include <locale.h>

struct timespec start, end, a, b;
extern double inPswap;
extern double inMemcpy;
extern double inHot;

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

void lineErode(image* g, LUT Ty, chordSet SE, int y){

    memset(g->img[y],UCHAR_MAX,g->W);

    for(size_t c=0;c<SE.size;c++){
        simdMinInPlace(g->img[y], Ty.arr[ SE.C[c].y ][ SE.C[c].i ] + SE.C[c].x, g->W);
	}
}

image erode(image f, chordSet SE){
	int y;
	image g;
	g.W = f.W;
	g.H = f.H;
	g.padX = 0;
	g.padY = 0;
	g.range = 255;
	allocateImage(&g,0,0,0);

    LUT Ty = computeLUT(f, SE);

    lineErode(&g, Ty, SE, 0);

    for(y=1;y< f.H; y++){
        updateLUT(f,&Ty,SE,y);
        lineErode(&g, Ty, SE, y);
	}

	freeLUT(Ty);
	return g;
}


int main(int argc, char* argv[]){
	if(argc < 3){
		printf("usage: UW image.pgm se.pbm\n");
		return 0;
	}

	image SEimg = readPGM(argv[2], 0, 0, 0);
    chordSet SE = buildChordSet(SEimg);
    freeImage(SEimg);

	image f = readPGM(argv[1], SE.R[SE.Lnum -1] - 1, 0 - SE.minY, SE.maxY>0?SE.maxY:0);

    image g = erode(f,SE);
	writePGM("out.pgm",g);

	freeChordSet(SE);
	freeImage(f);
	freeImage(g);

	return 0;
}
