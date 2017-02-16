#include "mex.h"
#include "matrix.h"
#include "matlabTransform.h"

void mexFunction(int nlhs, mxArray *plhs[],
                 int nrhs, const mxArray *prhs[]){
    matlabTransform(nlhs, plhs, nrhs, prhs, UW_DILATE);
}
