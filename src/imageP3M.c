#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "imageP3M.h"
#include <limits.h>

//TODO: max range isn't always 255, CHAR does not always work


//Kill me now it hurts to live.
void allocateImage3D(image3D* imp){
    size_t i,j;
    size_t sizeX = imp->W;
    size_t sizeY = imp->H;
    size_t sizeZ = imp->D;\

    imp->bytes = (unsigned char**)malloc(sizeZ * sizeof(char*));
    for(i = 0; i< sizeZ; i++){
        imp->bytes[i] = (unsigned char*)calloc(sizeY * sizeX, sizeof(char));
    }

    imp->img = (unsigned char***)malloc(sizeZ * sizeof(char**));
    for(i = 0; i < sizeZ; i++){
        imp->img[i] = (unsigned char**)malloc(sizeY * sizeof(char*));
        for(j = 0; j < sizeY; j++){
            imp->img[i][j] = (imp->bytes[i] + j*sizeX);

            assert(imp->img[i][j] != NULL);
        }
    }
}

void freeImage3D(image3D imp){
    free(imp.bytes);
    free(imp.img);
}

image3D readP3M(const char* filename){
    unsigned char c, fileType;
    size_t i, j, k;
    image3D im;

    FILE* ifp = fopen(filename,"r");

    c=getc(ifp);
    if(c != 'P'){
        printf("ERROR: Invalid file type\n");
        exit(1);
    }

    c=getc(ifp);
    fileType = c - '0';
    if(fileType != 8){
        // P8: Raw P3M
        printf("ERROR: Invalid file type\n");
        exit(1);
    }

    // Skipping comment lines
    while(getc(ifp) != '\n');
    while(getc(ifp) == '#'){
        while(getc(ifp) != '\n');
    }

    fseek(ifp, -1, SEEK_CUR);

    assert ( fscanf(ifp, "%lu", &(im.W)) == 1 );
    assert ( fscanf(ifp, "%lu", &(im.H)) == 1 );
    assert ( fscanf(ifp, "%lu", &(im.D)) == 1 );
    assert ( fscanf(ifp, "%lu\n", &(im.range)) == 1 );

    allocateImage3D(&im);

    for(i=0;i<im.D;i++){
        for(j=0;j<im.H;j++){
            for(k=0;k<im.W;k++){
                assert ( fscanf(ifp,"%hhu", &c) == 1 );
                im.img[i][j][k] = c;
            }
        }
    }

    fclose(ifp);

    //printf("???\n");
    return im;
}

void writeP3M(const char* filename, image3D im){
    FILE* ofp = fopen(filename,"w");

    size_t x,y,z;
    fprintf(ofp,"P8\n");
    fprintf(ofp,"%lu %lu %lu\n",im.W, im.H, im.D);
    fprintf(ofp,"%lu\n",im.range);
    for(z=0;z<im.D;z++){
        for(y=0;y<im.H;y++){
            for(x=0;x<im.W-1;x++){
                fprintf(ofp,"%d ",im.img[z][y][x]);
            }
            fprintf(ofp,"%d\n",im.img[z][y][im.W-1]);
        }
    }

    fclose(ofp);
}

image3D sphere(int d){
    image3D im;
    im.D = d;
    im.W = d;
    im.H = d;
    im.range = 1;

    float hd=d/2;

    allocateImage3D(&im);

    size_t x,y, z;
    for(z=0;z<im.D;z++){
        for(y=0;y<im.H;y++){
            for(x=0;x<im.W;x++){
                im.img[z][y][x] = sqrt((y-hd)*(y-hd) + (x-hd)*(x-hd)) < hd ? 1 : 0;
            }
        }
    }

    return im;
}

void printP3BM(image3D im){
    size_t z,y,x;

    for(z=0;z<im.D;z++){
        for(y=0;y<im.H;y++){
            for(x=0;x<im.W;x++){
                putchar(im.img[z][y][x] == 1 ? '=' : '.');
            }
            putchar('\n');
        }
    }
}
