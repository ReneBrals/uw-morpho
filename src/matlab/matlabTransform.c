#include "../imagePGM.h"
#include "../chordSet.h"
#include "../transform.h"
#include "matlabTransform.h"

void columnToRowMajorOrder(unsigned char* buf, image f, size_t x, size_t y){
    //copies a column-major order buffer into a row-major image.

    for(size_t i = 0; i < x * y; i++){
        f.img[i%y][i/y] = buf[i];
    }
}

void rowtoColumnMajorOrder(unsigned char* buf, image f, size_t x, size_t y){
    //copies a column-major order buffer into a row-major image.

    for(size_t j = 0; j < y; j++){
        for(size_t i = 0; i < x; i++){
            buf[j+y*i] = f.img[j][i];
        }
    }
}

void matlabTransform(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[], int mode){
    mwSize ncolsF, nrowsF, ncolsSE, nrowsSE;
    image f, SEimg, g;
    chordSet SE;

    if(nrhs != 2){
        mexErrMsgIdAndTxt("MyToolbox:uwErode:nrhs",
                      "Two inputs required.");
    }

    if(nlhs > 1){
        mexErrMsgIdAndTxt("MyToolbox:uwErode:nlhs",
                      "One output required.");
    }

    if(!mxIsUint8(prhs[0])){
        mexErrMsgIdAndTxt("MyToolbox:uwErode:notUint8",
        "Input image must be type uint8.");

    }

    if(!mxIsUint8(prhs[1])){
        mexErrMsgIdAndTxt("MyToolbox:uwErode:notUint8",
        "Input structuring element must be type uint8.");

    }

    nrowsF = mxGetM(prhs[0]);
    ncolsF = mxGetN(prhs[0]);

    nrowsSE = mxGetM(prhs[1]);
    ncolsSE = mxGetN(prhs[1]);

    f = initImage(ncolsF,nrowsF,255);
    columnToRowMajorOrder((unsigned char*)mxGetData(prhs[0]), f, ncolsF, nrowsF);

    SEimg = initImage(ncolsSE, nrowsSE, 1);
    columnToRowMajorOrder((unsigned char*)mxGetData(prhs[1]), SEimg, ncolsSE, nrowsSE);

    plhs[0] = mxCreateNumericMatrix(nrowsF,ncolsF,mxUINT8_CLASS,mxREAL);

    unsigned char* gImg = (unsigned char*)mxGetData(plhs[0]);

    g = initImagePreallocated(gImg,ncolsF,nrowsF,1);

    SE = buildChordSet(SEimg);

    g = initImage(ncolsF, nrowsF, 255);

    if(mode == UW_ERODE){
        erode(&g,f,SE);
    } else if (mode == UW_DILATE){
        dilate(&g,f,SE);
    }

    rowtoColumnMajorOrder((unsigned char*)mxGetData(plhs[0]),g,ncolsF,nrowsF);

    freeImage(f);
    freeImage(g);
    freeImage(SEimg);

    freeChordSet(SE);


}
