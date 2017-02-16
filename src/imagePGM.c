#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "imagePGM.h"
#include <limits.h>
#include "unistd.h"

//TODO: max range isn't always 255, CHAR does not always work

void allocateImage(image* imp){

    imp->bytes = (unsigned char*)calloc((imp->H) * (imp->W), sizeof(char));
    imp->img = (unsigned char**)malloc((imp->H) * sizeof(char*));

    for(size_t i = 0; i < (imp->H); i++){
        imp->img[i] = (imp->bytes + i*(imp->W));

        assert(imp->img[i] != NULL);
    }
}

void freeImage(image imp){
    free(imp.bytes);
    free(imp.img);
}

image initImage(size_t w, size_t h, size_t range){
    image g;
    g.W = w;
    g.H = h;
    g.range = 255;
    allocateImage(&g);
    return g;
}

image initImagePreallocated(unsigned char* x, size_t w, size_t h, size_t range){
    image g;
    g.W = w;
    g.H = h;
    g.range = 255;
    g.bytes = x;

    g.img = (unsigned char**)malloc((g.H) * sizeof(char*));

    for(size_t i = 0; i < (g.H); i++){
        g.img[i] = (g.bytes + i*(g.W));

        assert(g.img[i] != NULL);
    }
    return g;
}

image readPGM(const char* filename){
    unsigned char c, fileType;
    size_t i, j, k;
    image im;

    if( access( filename, F_OK ) == -1 ){
        printf("ERROR: file %s does not exist\n",filename);
        exit(1);
    }

    FILE* ifp = fopen(filename,"r");

    c=getc(ifp);
    if(c != 'P'){
        printf("ERROR: Invalid file type\n");
        exit(1);
    }

    c=getc(ifp);
    fileType = c - '0';
    if(fileType != 1 && fileType != 2 && fileType != 4 && fileType != 5){
        // P1: ASCII PBM
        // P2: ASCII PGM
        // P4: Raw PBM
        // P5: Raw PGM
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

    if(fileType == 2 || fileType == 5){ // PGM has a maxval, PBM doesn't
        assert ( fscanf(ifp, "%lu", &(im.range)) == 1 );
    } else {
        im.range = 1;
    }

    allocateImage(&im);

    if(fileType == 1 || fileType == 2){ // ASCII PBM/PGMs can be scanf'd
        for(i = 0; i < im.H; i++){
            for(j = 0; j < im.W;j++){
                assert ( fscanf(ifp,"%hhu", &c) == 1 );
                im.img[i][j] = c;
            }
        }
    } else if (fileType == 4){
        for(i=0;i<im.H;i++){
            for(j=0;j<(im.W-1)/8;j++){
                c = getc(ifp);
                for(k=7; k>=0; k--){
                    im.img[i][j*8 + k] = (c >> k) & 0x01;
                }
            }

            c = getc(ifp);
            for(k = 0; k < im.W % 8; k++){
                im.img[i][((im.W * 8) / 8) + k] = (c >> (7-k)) & 0x01;
            }
        }
    } else {
        for(i=0;i<im.H;i++){
            for(j=0;j<im.W;j++){
                c = getc(ifp);
                im.img[i][j] = c;
            }
        }
    }

    fclose(ifp);

    //printf("???\n");
    return im;
}

void writePGM(const char* filename, image im){
    FILE* ofp = fopen(filename,"w");

    size_t x,y;
    fprintf(ofp,"P2\n");
    fprintf(ofp,"%lu %lu\n",im.W,im.H);
    fprintf(ofp,"%lu\n",im.range);
    for(y=0;y<im.H;y++){
        for(x=0;x<im.W-1;x++){
            fprintf(ofp,"%d ",im.img[y][x]);
        }
        fprintf(ofp,"%d\n",im.img[y][im.W-1]);
    }

    fclose(ofp);
}

image disk(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    float hd=d/2;

    allocateImage(&im);

    size_t x,y;
    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            im.img[y][x] = sqrt((y-hd)*(y-hd) + (x-hd)*(x-hd)) < hd ? 1 : 0;
        }
    }

    im.img[im.H/2][im.W/2] = 1;

    return im;
}

image rectangle(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    allocateImage(&im);

    size_t x,y;
    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            im.img[y][x] = 1;
        }
    }

    return im;
}

image h(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    allocateImage(&im);

    size_t x,y;

    for(x=0;x<im.W;x++){
        im.img[im.H/2][x] = 1;
    }

    for(y=0;y<im.H;y++){
        im.img[y][0] = 1;
        im.img[y][im.W - 1] = 1;
    }

    return im;
}

image checkerboard(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    allocateImage(&im);

    size_t x,y;

    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            im.img[y][x] = (x+y % 2 == 0);
        }
    }

    return im;
}

image noise(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    allocateImage(&im);

    size_t x,y;

    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            im.img[y][x] = rand() % 2;
        }
    }

    im.img[im.H/2][im.W/2] = 1;

    return im;
}

void printPBM(image im){
    size_t y,x;

    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            putchar(im.img[y][x] == 1 ? '=' : '.');
        }
        putchar('\n');
    }

}
