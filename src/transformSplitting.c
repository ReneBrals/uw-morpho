/* transformSplitting.c
 * Morphological transformations on images (alternate method of parallelization).
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "LUTSplitting.h"
#include "SIMD.h"
#include "transform.h"
#include <omp.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void lineErode(image* g, LUT Ty, chordSet SE, int y){
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Erodes a line by initializing it to 0, then for each pixel finding the
     * minimum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */
    memset(g->img[y], UCHAR_MAX, g->W);

    for(size_t c = 0;c < SE.size;c++){
        simdMinInPlace(g->img[y],
            Ty.arr[ SE.C[c].y ][ SE.C[c].i ] + SE.C[c].x,
            MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void lineDilate(image* g, LUT Ty, chordSet SE, int y){
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Dilates a line by initializing it to 0, then for each pixel finding the
     * maximum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */
    memset(g->img[y], 0, g->W);

    for(size_t c = 0;c < SE.size; c++){
        simdMaxInPlace(g->img[y],
            Ty.arr[ SE.C[c].y ][ SE.C[c].i ] + SE.C[c].x,
            MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void erode(image* g, image f, chordSet SE){
    /*
     * Algorithm II.3 in the Urbach-Wilkinson paper.
     * Computes the erosion g = erode(f,SE).
     *
     * This implementation is parallelized by splitting the image.
     */

	size_t num;
	#pragma omp parallel
	{
		#pragma omp single
		{
			num = omp_get_num_threads();
		}
	}

    LUT Ty = computeMinLUT(f, SE, 0);

    if(num > 1 && f.H >= num){
        #pragma omp parallel
        {
            size_t tid = omp_get_thread_num();

            size_t minY = (tid * f.H) / num;
            size_t maxY;
            if(tid != num-1){
                maxY = ((tid+1) * f.H) / num;
            } else {
                maxY = f.H;
            }

            LUT Ty = computeMinLUT(f, SE, minY);

            lineErode(g, Ty, SE, minY);
            for(size_t y = minY; y < maxY; y++){
                updateMinLUT(f,&Ty,SE,y);
                lineErode(g, Ty, SE, y);
            }

            freeLUT(Ty);
        }
    } else {
        /*
         * If there is only one thread, or not enough lines for all threads,
         * just fall back to serial computation.
         */
         LUT Ty = computeMinLUT(f, SE, 0);
         lineErode(g, Ty, SE, 0);
         for(size_t y = 1; y < f.H; y++){
             updateMinLUT(f,&Ty,SE,y);
             lineErode(g, Ty, SE, y);
         }
         freeLUT(Ty);
    }

    freeLUT(Ty);
}

void dilate(image* g, image f, chordSet SE){
    /*
     * Algorithm II.3 in the Urbach-Wilkinson paper.
     * Computes the dilation g = dilate(f,SE).
     *
     * This implementation is parallelized in strides: the lookup table contains
     * additional r-indices such that multiple consecutive lines can be dilated
     * in parallel.
     */

	size_t num;
	#pragma omp parallel
	{
		#pragma omp single
		{
			num = omp_get_num_threads();
		}
	}

    LUT Ty = computeMaxLUT(f, SE, 0);

    if(num > 1 && f.H >= num){
        #pragma omp parallel
        {
            size_t tid = omp_get_thread_num();

            size_t minY = (tid * f.H) / num;
            size_t maxY;
            if(tid != num-1){
                maxY = ((tid+1) * f.H) / num;
            } else {
                maxY = f.H;
            }

            LUT Ty = computeMaxLUT(f, SE, minY);

            lineDilate(g, Ty, SE, minY);
            for(size_t y = minY; y < maxY; y++){
                updateMaxLUT(f,&Ty,SE,y);
                lineDilate(g, Ty, SE, y);
            }

            freeLUT(Ty);
        }
    } else {
        /*
         * If there is only one thread, or not enough lines for all threads,
         * just fall back to serial computation.
         */
         LUT Ty = computeMaxLUT(f, SE, 0);
         lineDilate(g, Ty, SE, 0);
         for(size_t y = 1; y < f.H; y++){
             updateMaxLUT(f,&Ty,SE,y);
             lineDilate(g, Ty, SE, y);
         }
         freeLUT(Ty);
    }

    freeLUT(Ty);
}

void imageDiffInPlace(image f, image g){
    #pragma omp parallel for
    for(size_t y = 0;y<f.H;y++){
        simdSubInPlace(f.img[y],g.img[y],f.W);
    }
}

/*
 * Some additional operations that are easily expressed using erosion/dilation.
 */

void open(image* g, image f, chordSet SE){
    erode(g, f, SE);
    image h = initImage(f.W, f.H, f.range);
    dilate(&h, *g, SE);
    freeImage(*g);
    *g = h;
}

void close(image* g, image f, chordSet SE){
    dilate(g, f, SE);
    image h = initImage(f.W, f.H, f.range);
    erode(&h, *g, SE);
    freeImage(*g);
    *g = h;
}

void whiteTopHat(image* g, image f, chordSet SE){
    /*
     * Note that this outputs the transform into f instead of g, we'll work
     * around that at the I/O level to prevent having to copy stuff over.
     */
	open(g, f, SE);
	imageDiffInPlace(f,*g);
}

void blackTopHat(image* g, image f, chordSet SE){
    close(g, f, SE);
	imageDiffInPlace(*g, f);
}
