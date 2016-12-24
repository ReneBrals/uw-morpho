

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "imagePGM.h"

//TODO: max range isn't always 255, CHAR does not always work

void allocateImage(image* imp, int x, int preY, int postY){

    imp->bytes = (unsigned char*)calloc((imp->H + preY + postY) * (imp->W + x), sizeof(char));
    imp->img = (unsigned char**)malloc((imp->H + preY + postY) * sizeof(char*));

    for(int i = 0; i < (imp->H + preY + postY); i++){
        imp->img[i] = (imp->bytes + i*(imp->W + x));
        assert(imp->img[i] != NULL);
    }

    if(preY > 0){
        imp->img  = &(imp->img[preY]);
    }
}

void freeImage(image imp){
    free(imp.bytes);
    free(&(imp.img[0-imp.padY]));
}

image readPGM(char* filename, int padx, int prePadY, int postPadY){
    unsigned char c, fileType;
    int i,j,k;
    image im;

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

    assert ( fscanf(ifp, "%d", &(im.W)) == 1 );
    assert ( fscanf(ifp, "%d", &(im.H)) == 1 );

    if(fileType == 2 || fileType == 5){ // PGM has a maxval, PBM doesn't
        assert ( fscanf(ifp, "%d", &(im.range)) == 1 );
    } else {
        im.range = 1;
    }

    im.padX = padx;
    im.padY = prePadY;

    allocateImage(&im, padx, prePadY, postPadY);

    if(fileType == 1 || fileType == 2){ // ASCII PBM/PGMs can be scanf'd
        for(i = 0; i < im.H; i++){
            for(j = 0; j < im.W;j++){
                assert ( fscanf(ifp,"%hhu", &c) == 1 );
                im.img[i][j] = c;
                //printf("%d\n",im.img[i][j]);
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

void writePGM(char* filename, image im){
    FILE* ofp = fopen(filename,"w");

    int x,y;
    fprintf(ofp,"P2\n");
    fprintf(ofp,"%d %d\n",im.W,im.H);
    fprintf(ofp,"%d\n",im.range);
    for(y=0;y<im.H;y++){
        for(x=0;x<im.W-1;x++){
            fprintf(ofp,"%d ",im.img[y+im.padY][x+im.padX]);
        }
        fprintf(ofp,"%d\n",im.img[y+im.padY][im.W-1+im.padX]);
    }

    fclose(ofp);
}

image disk(int d){
    image im;
    im.W = d;
    im.H = d;
    im.range = 1;

    float hd=d/2;

    allocateImage(&im,0,0,0);

    int x,y;
    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            im.img[y][x] = sqrt((y-hd)*(y-hd) + (x-hd)*(x-hd)) < hd ? 1 : 0;
        }
    }

    return im;
}

void printPBM(image im){
    int y,x;

    for(y=0;y<im.H;y++){
        for(x=0;x<im.W;x++){
            putchar(im.img[y][x] == 1 ? '=' : '.');
        }
        putchar('\n');
    }

}
