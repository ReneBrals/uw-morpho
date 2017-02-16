#ifndef _MATLAB_TRANSFORM
#define _MATLAB_TRANSFORM_
#include "mex.h"
#include "matrix.h"

#define UW_ERODE 0
#define UW_DILATE 1
#define UW_OPEN 2
#define UW_CLOSE 3

void matlabTransform(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

#endif
