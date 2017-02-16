# uw-morpho
Implementation of the Urbach-Wilkinson<sup>1</sup> algorithm for grayscale morphological operations with arbitrary flat structuring elements. Supports 2D and 3D images in PGM format and can be used within MATLAB.

Makes use of vector intrinsics and OpenMP to increase performance. 

## Build Instructions
### Standalone program

`make`

`make fast` if CPU supports AVX2 (Haswell or newer) for slightly better performance.

`make slow` if CPU does not support SSE2.

### MATLAB MEX file

Execute the `build_uw_morpho.m` script within MATLAB.



## Usage

### Standalone program

2D operations: `./uw2d (erode|dilate|open|close) image.pgm SE.pbm`

3D operations: `./uw3d (erode|dilate|open|close) image.p3m SE.p3m`

### MATLAB

`erode(f,SE)`, `dilate(f,SE)`, `open(f,SE)` and `close(f,SE)`

**Supports uint8 only**

<sup>1. Urbach, E., & Wilkinson, M. (2006). Efficient 2-D Gray-Scale Dilations and Erosions with Arbitrary Flat Structuring Elements. In 2006 International Conference on Image Processing (Vol. 17, pp. 1573–1576). IEEE. http://doi.org/10.1109/ICIP.2006.312608</sup>
