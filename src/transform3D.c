/* transform.c
 * Morphological transformations on 3D images.
 */

#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "LUT3D.h"
#include "SIMD.h"
#include "transform3D.h"

void lineErode3D(image3D* g, LUT3D Ty, chordSet3D SE, int y, int z){
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Erodes a line by initializing it to 0, then for each pixel finding the
     * minimum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */

    memset(g->img[z][y], UCHAR_MAX, g->W);
    for(size_t c = 0;c < SE.size; c++){
        simdMinInPlace(g->img[z][y],
            Ty.arr[ SE.C[c].y ][ SE.C[c].z ][ SE.C[c].i ] + SE.C[c].x,
            g->W);
	}
}

void lineDilate3D(image3D* g, LUT3D Ty, chordSet3D SE, int y, int z){
    /*
     * Algorithm II.2 in the Urbach-Wilkinson paper.
     * Dilates a line by initializing it to 0, then for each pixel finding the
     * maximum pixel value covered by chords in the SE from the lookup table.
     * This is done with some SIMD vector magic.
     */

    memset(g->img[y],0,g->W);
    for(size_t c = 0;c < SE.size; c++){
        simdMaxInPlace(g->img[z][y],
            Ty.arr[ SE.C[c].y ][ SE.C[c].z ][ SE.C[c].i ] + SE.C[c].x,
            g->W);
	}
}

void imageDiffInPlace3D(image3D f, image3D g){
    for(size_t z = 0; z < f.D; z++){
        for(size_t y = 0; y < f.H; y++){
            simdSubInPlace(f.img[z][y], g.img[z][y], f.W);
        }
    }
}

void planeErode3D(image3D f, image3D* g, chordSet3D SE, int z){
    size_t y;

    LUT3D Ty = computeMinLUT3D(f, SE, z);
    lineErode3D(g, Ty, SE, 0, z);
    for(y = 1; y < f.H; y++){
        updateMinLUT3D(f, &Ty, SE, y, z);
        lineErode3D(g, Ty, SE, y, z);
    }

    freeLUT3D(Ty);
}

void planeDilate3D(image3D f, image3D* g, chordSet3D SE, int z){
    size_t y;

    LUT3D Ty = computeMaxLUT3D(f, SE, z);
    lineDilate3D(g, Ty, SE, 0, z);
    for(y = 1; y < f.H; y++){
        updateMaxLUT3D(f, &Ty, SE, y, z);
        lineDilate3D(g, Ty, SE, y, z);
    }

    freeLUT3D(Ty);
}

image3D erode3D(image3D f, chordSet3D SE){
    size_t z;
    image3D g;
    g.W = f.W;
    g.H = f.H;
    g.D = f.D;
    g.range = 255;
    allocateImage3D(&g);

    for(z = 0; z < f.D; z++){
        planeErode3D(f, &g, SE, z);
    }
    return g;
}

image3D dilate3D(image3D f, chordSet3D SE){
    size_t z;
    image3D g;
    g.W = f.W;
    g.H = f.H;
    g.D = f.D;
    g.range = 255;
    allocateImage3D(&g);

    for(z = 0; z < f.D; z++){
        planeDilate3D(f, &g, SE, z);
    }
    return g;
}

/*
 * Some additional operations that are easily expressed using erosion/dilation.
 */

image3D open3D(image3D f, chordSet3D SE){
	image3D g = erode3D(f, SE);
	image3D h = dilate3D(g, SE);
	freeImage3D(g);
	return h;
}

image3D close3D(image3D f, chordSet3D SE){
	image3D g = dilate3D(f, SE);
	image3D h = erode3D(g, SE);
	freeImage3D(g);
	return h;
}

image3D whiteTopHat3D(image3D f, chordSet3D SE){
    /*
     * Note that this outputs the transform into f instead of g, we'll work
     * around that at the I/O level to prevent having to copy stuff over.
     */
	image3D g = open3D(f,SE);
	imageDiffInPlace3D(f,g);
	return g;
}

image3D blackTopHat(image3D f, chordSet3D SE){
	image3D g = close3D(f,SE);
	imageDiffInPlace3D(g,f);
	return g;
}
