#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "LUT.h"
#include "SIMD.h"
#include "transform.h"
#include <omp.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void lineErode(image* g, LUT Ty, chordSet SE, int y, int tid){
    memset(g->img[y],UCHAR_MAX,g->W);
    for(size_t c=0;c<SE.size;c++){
        simdMinInPlace(g->img[y], Ty.arr[ SE.C[c].y + tid ][ SE.C[c].i ] + SE.C[c].x, MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void lineDilate(image* g, LUT Ty, chordSet SE, int y){
    memset(g->img[y],0,g->W);
    for(size_t c=0;c<SE.size;c++){
        simdMaxInPlace(g->img[y], Ty.arr[ SE.C[c].y ][ SE.C[c].i ] + SE.C[c].x, MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void imageDiffInPlace(image f, image g){
    for(size_t y = 0;y<f.H;y++){
        simdSubInPlace(f.img[y],g.img[y],f.W);
    }
}

image erode(image f, chordSet SE){
    image g;
    g.W = f.W;
    g.H = f.H;
    g.range = 255;
    allocateImage(&g);

    LUT Ty = computeMinLUT(f, SE, 0);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        int num = omp_get_num_threads();

        lineErode(&g, Ty, SE, tid, tid);
        for(size_t y = num; y < (f.H/num)*num; y = y + num){
            #pragma omp barrier

            updateMinLUT(f,&Ty,SE,y, tid, num);

            #pragma omp barrier

            lineErode(&g, Ty, SE, y + tid, tid);
        }
    }

    int num = omp_get_max_threads();

    for(size_t y = (f.H/num)*num; y < f.H; y++){
        updateMinLUT(f,&Ty,SE,y, num-1, 1);
        lineErode(&g, Ty, SE, y, num-1);
    }

    freeLUT(Ty);

    return g;
}

image dilate(image f, chordSet SE){
    size_t y;
    image g;
    g.W = f.W;
    g.H = f.H;
    g.range = 255;
    allocateImage(&g);

    LUT Ty = computeMaxLUT(f, SE, 0);
    lineDilate(&g, Ty, SE, 0);
    for(y=1;y< f.H; y++){
        updateMaxLUT(f,&Ty,SE,y);
        lineDilate(&g, Ty, SE, y);
    }

    freeLUT(Ty);
    return g;
}

image open(image f, chordSet SE){
	image g = erode(f, SE);
	image h = dilate(g, SE);
	freeImage(g);
	return h;
}

image close(image f, chordSet SE){
	image g = dilate(f, SE);
	image h = erode(g, SE);
	freeImage(g);
	return h;
}

image whiteTopHat(image f, chordSet SE){
	image g = open(f,SE);
	imageDiffInPlace(f,g);
	return g;
}

image blackTopHat(image f, chordSet SE){
	image g = close(f,SE);
	imageDiffInPlace(g,f);
	return g;
}
