#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "LUT.h"
#include "SIMD.h"
#include "transform.h"
#include <omp.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void lineErode(image* g, LUT Ty, chordSet SE, int y, size_t tid){
    memset(g->img[y],UCHAR_MAX,g->W);
    for(size_t c=0;c<SE.size;c++){
        simdMinInPlace(g->img[y], Ty.arr[ SE.C[c].y + tid ][ SE.C[c].i ] + SE.C[c].x, MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void lineDilate(image* g, LUT Ty, chordSet SE, int y, size_t tid){
    memset(g->img[y],0,g->W);
    for(size_t c=0;c<SE.size;c++){
        simdMaxInPlace(g->img[y], Ty.arr[ SE.C[c].y + tid ][ SE.C[c].i ] + SE.C[c].x, MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void imageDiffInPlace(image f, image g){
    #pragma omp parallel for
    for(size_t y = 0;y<f.H;y++){
        simdSubInPlace(f.img[y],g.img[y],f.W);
    }
}

void erode(image* g, image f, chordSet SE){
	size_t num;
	#pragma omp parallel
	{
		#pragma omp single
		{
			num = omp_get_num_threads();
		}
	}

    LUT Ty = computeMinLUT(f, SE, 0, num);

    if(f.H >= num){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();

            lineErode(g, Ty, SE, tid, tid);
            for(size_t y = num; y < (f.H/num)*num; y = y + num){
                #pragma omp barrier

                updateMinLUT(f,&Ty,SE,y, tid, num);

                #pragma omp barrier

                lineErode(g, Ty, SE, y + tid, tid);
            }
        }

        for(size_t y = (f.H/num)*num; y < f.H; y++){
            updateMinLUT(f,&Ty,SE, y-num+1, 0, 1);
            lineErode(g, Ty, SE, y, num-1);
        }
    } else {
        lineErode(g, Ty, SE, 0, 0);
        for(size_t y = 1; y < f.H; y++){
            updateMinLUT(f,&Ty,SE,y, 0, 1);
            lineErode(g, Ty, SE, y, 0);
        }
    }

    freeLUT(Ty);
}

void dilate(image* g, image f, chordSet SE){

	size_t num;
	#pragma omp parallel
	{
		#pragma omp single
		{
			num = omp_get_num_threads();
		}
	}

    LUT Ty = computeMaxLUT(f, SE, 0, num);

    if(f.H >= num){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();

            lineDilate(g, Ty, SE, tid, tid);
            for(size_t y = num; y < (f.H/num)*num; y = y + num){
                #pragma omp barrier

                updateMaxLUT(f,&Ty,SE,y, tid, num);

                #pragma omp barrier

                lineDilate(g, Ty, SE, y + tid, tid);
            }
        }

        for(size_t y = (f.H/num)*num; y < f.H; y++){
            updateMaxLUT(f,&Ty,SE, y-num+1, 0, 1);
            lineDilate(g, Ty, SE, y, num-1);
        }
    } else {
        lineDilate(g, Ty, SE, 0, 0);
        for(size_t y = 1; y < f.H; y++){
            updateMaxLUT(f,&Ty,SE,y, 0, 1);
            lineDilate(g, Ty, SE, y, 0);
        }
    }

    freeLUT(Ty);
}

void open(image* g, image f, chordSet SE){
    erode(g, f, SE);
    image h = initImage(f.W,f.H,f.range);
    dilate(&h, *g, SE);
    freeImage(*g);
    *g = h;
}

void close(image* g, image f, chordSet SE){
    dilate(g, f, SE);
    image h = initImage(f.W,f.H,f.range);
    erode(&h, *g, SE);
    freeImage(*g);
    *g = h;
}

void whiteTopHat(image* g, image f, chordSet SE){
	open(g, f,SE);
	imageDiffInPlace(f,*g);
}

void blackTopHat(image* g, image f, chordSet SE){
    close(g, f,SE);
	imageDiffInPlace(f,*g);
}
