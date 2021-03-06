/* transform.c
 * Morphological transformations on images.
 */

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
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Erodes a line by initializing it to 0, then for each pixel finding the
     * minimum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */
    memset(g->img[y], UCHAR_MAX, g->W);

    for(size_t c = 0;c < SE.size;c++){
        /* Note that the r-index of Ty is shifted by the tid: the lookup table
         * for one row is a slightly smaller shifted lookup table.
         */
        simdMinInPlace(g->img[y],
            Ty.arr[ SE.C[c].y + tid ][ SE.C[c].i ] + SE.C[c].x,
            MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void lineDilate(image* g, LUT Ty, chordSet SE, int y, size_t tid){
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Dilates a line by initializing it to 0, then for each pixel finding the
     * maximum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */
    memset(g->img[y], 0, g->W);

    for(size_t c = 0;c < SE.size; c++){
        /* Note that the r-index of Ty is shifted by the tid: the lookup table
         * for one row is a slightly smaller shifted lookup table.
         */
        simdMaxInPlace(g->img[y],
            Ty.arr[ SE.C[c].y + tid ][ SE.C[c].i ] + SE.C[c].x,
            MIN( MAX( (int)g->W - SE.C[c].x, 0), g->W));
	}
}

void erode(image* g, image f, chordSet SE){
    /*
     * Algorithm II.3 in the Urbach-Wilkinson paper.
     * Computes the erosion g = erode(f,SE).
     *
     * This implementation is parallelized in strides: the lookup table contains
     * additional r-indices such that multiple consecutive lines can be eroded
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

    LUT Ty = computeMinLUT(f, SE, 0, num);

    if(num > 1 && f.H >= num){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();

            lineErode(g, Ty, SE, tid, tid);
            for(size_t y = num; y < ( f.H / num ) * num; y = y + num){
                #pragma omp barrier

                updateMinLUT(f, &Ty, SE, y, tid, num);

                #pragma omp barrier

                lineErode(g, Ty, SE, y + tid, tid);
            }
        }

        for(size_t y = (f.H/num)*num; y < f.H; y++){
            /*
             * Annoying boundary behavior: when the last few lines are done
             * serially, we need to update the LUT it as the first thread ID,
             * but erode as the last thread ID.
             */
            updateMinLUT(f, &Ty, SE, y - num + 1, 0, 1);
            lineErode(g, Ty, SE, y, num - 1);
        }
    } else {
        /*
         * If there is only one thread, or not enough lines for all threads,
         * just fall back to serial computation.
         */
        lineErode(g, Ty, SE, 0, 0);
        for(size_t y = 1; y < f.H; y++){
            updateMinLUT(f, &Ty, SE, y, 0, 1);
            lineErode(g, Ty, SE, y, 0);
        }
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

    LUT Ty = computeMaxLUT(f, SE, 0, num);

    if(num > 1 && f.H >= num){
        #pragma omp parallel
        {
            int tid = omp_get_thread_num();

            lineDilate(g, Ty, SE, tid, tid);
            for(size_t y = num; y < (f.H/num)*num; y = y + num){
                #pragma omp barrier

                updateMaxLUT(f, &Ty, SE, y, tid, num);

                #pragma omp barrier

                lineDilate(g, Ty, SE, y + tid, tid);
            }
        }

        for(size_t y = (f.H / num) * num; y < f.H; y++){
            /*
             * Annoying boundary behavior: when the last few lines are done
             * serially, we need to update the LUT it as the first thread ID,
             * but erode as the last thread ID.
             */
            updateMaxLUT(f, &Ty, SE, y - num + 1, 0, 1);
            lineDilate(g, Ty, SE, y, num - 1);
        }
    } else {
        /*
         * If there is only one thread, or not enough lines for all threads,
         * just fall back to serial computation.
         */
        lineDilate(g, Ty, SE, 0, 0);
        for(size_t y = 1; y < f.H; y++){
            updateMaxLUT(f, &Ty, SE, y, 0, 1);
            lineDilate(g, Ty, SE, y, 0);
        }
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
