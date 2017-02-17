#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "transform.h"
#include "chordSet.h"
#include "imagePGM.h"
#include "LUT.h"
#include <omp.h>
#include "time.h"
#include "unistd.h"

#define N 1
#define SE_SIZE 100

#ifdef __AVX2__
#define VECTORIZED 32
#elif __SSE2__
#define VECTORIZED 16
#else
#define VECTORIZED 1
#endif

int main(int argc, char* argv[]){
	if(argc < 2){
		printf("usage: bench image.pgm\n");
		return 0;
	}

    image SEimg;
    chordSet SE;

    double tick, tock;

    FILE * fp;

    image g, f;
	f = readImage(argv[1]);
	g = initImage(f.W, f.H, f.range);

    if( access( "circular.csv", F_OK ) != -1 ) {
        fp = fopen ("circular.csv", "a");
    } else {
        fp = fopen ("circular.csv", "w");
    }
    printf("Benchmarking circular SE up to size = %d\n", SE_SIZE);
    for(int n=1; n<=N; n*=2){
        printf("Nthreads = %d\n",n);
        for(int i=5; i<=SE_SIZE; i+=5){
			printf("i=%d\n",i);
            omp_set_num_threads(n);
            SEimg = disk(i);
            SE = buildChordSet(SEimg);
            freeImage(SEimg);

            tick = omp_get_wtime();
            erode(&g, f, SE);
            tock = omp_get_wtime();
            fprintf(fp,"%d, %d, %d, %f\n",n,VECTORIZED,i,tock-tick);
            freeChordSet(SE);
        }
    }
    fclose(fp);

    if( access( "rectangle.csv", F_OK ) != -1 ) {
        fp = fopen ("rectangle.csv", "a");
    } else {
        fp = fopen ("rectangle.csv", "w");
    }
    printf("Benchmarking rectangular SE up to size = %d\n", SE_SIZE);
    for(int n=1; n<=N; n*=2){
        printf("Nthreads = %d\n",n);
        for(int i=5; i<=SE_SIZE; i+=5){
			printf("i=%d\n",i);
            omp_set_num_threads(n);
            SEimg = rectangle(i);
            SE = buildChordSet(SEimg);
            freeImage(SEimg);

            tick = omp_get_wtime();
            erode(&g, f, SE);
            tock = omp_get_wtime();
            fprintf(fp,"%d, %d, %d, %f\n",n,VECTORIZED,i,tock-tick);
            freeChordSet(SE);
        }
    }
    fclose(fp);

    if( access( "h.csv", F_OK ) != -1 ) {
        fp = fopen ("h.csv", "a");
    } else {
        fp = fopen ("h.csv", "w");
    }
    printf("Benchmarking H-shaped SE up to size = %d\n", SE_SIZE);
    for(int n=1; n<=N; n*=2){
        printf("Nthreads = %d\n",n);
        for(int i=5; i<=SE_SIZE; i+=5){
			printf("i=%d\n",i);
            omp_set_num_threads(n);
            SEimg = h(i);
            SE = buildChordSet(SEimg);
            freeImage(SEimg);

            tick = omp_get_wtime();
            erode(&g, f, SE);
            tock = omp_get_wtime();
            fprintf(fp,"%d, %d, %d, %f\n",n,VECTORIZED,i,tock-tick);
            freeChordSet(SE);
        }
    }
    fclose(fp);

    if( access( "checkerboard.csv", F_OK ) != -1 ) {
        fp = fopen ("checkerboard.csv", "a");
    } else {
        fp = fopen ("checkerboard.csv", "w");
    }
    printf("Benchmarking checkerboard SE up to size = %d\n", SE_SIZE);
    for(int n=1; n<=N; n*=2){
        printf("Nthreads = %d\n",n);
        for(int i=5; i<=SE_SIZE; i+=5){
			printf("i=%d\n",i);
            omp_set_num_threads(n);
            SEimg = checkerboard(i);
            SE = buildChordSet(SEimg);
            freeImage(SEimg);

            tick = omp_get_wtime();
            erode(&g, f, SE);
            tock = omp_get_wtime();
            fprintf(fp,"%d, %d, %d, %f\n",n,VECTORIZED,i,tock-tick);
            freeChordSet(SE);
        }
    }
    fclose(fp);

    if( access( "noise.csv", F_OK ) != -1 ) {
        fp = fopen ("noise.csv", "a");
    } else {
        fp = fopen ("noise.csv", "w");
    }
    printf("Benchmarking noise SE up to size = %d\n", SE_SIZE);
    for(int n=1; n<=N; n*=2){
        printf("Nthreads = %d\n",n);
        for(int i=5; i<=SE_SIZE; i+=5){
			printf("i=%d\n",i);
            omp_set_num_threads(n);
            SEimg = noise(i);
            SE = buildChordSet(SEimg);
            freeImage(SEimg);

            tick = omp_get_wtime();
            erode(&g, f, SE);
            tock = omp_get_wtime();
            fprintf(fp,"%d, %d, %d, %f\n",n,VECTORIZED,i,tock-tick);
            freeChordSet(SE);
        }
    }
    fclose(fp);

	freeImage(f);
    freeImage(g);
	return 0;
}
