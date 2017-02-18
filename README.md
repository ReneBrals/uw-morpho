# uw-morpho
Implementation of the Urbach-Wilkinson<sup>1</sup> algorithm for grayscale morphological operations with arbitrary flat structuring elements. Supports 2D images in JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM, PPM and PGM file formats through [stb](https://github.com/nothings/stb) and 3D images in P3M. 2D transformations can be used within MATLAB.

Makes use of vector intrinsics and OpenMP to increase performance. 

## Build Instructions
### Standalone program

`make`

`make fast` if CPU supports AVX2 (Haswell or newer) for slightly better performance.

`make slow` if CPU does not support SSE2.

By default, the program will transform lines in stride to achieve parallelism. To split the image in blocks instead, compile with `make split`, `make splitfast` or `make splitslow`.

### MATLAB MEX file

Execute the `build_uw_morpho.m` script within MATLAB.

## Usage

### Standalone program

2D operations: `./uw2d (erode|dilate|open|close) image_file SE_file`

Supports 2D images in JPG, PNG, TGA, BMP, PSD, GIF, HDR, PIC, PNM, PPM and PGM file formats.

3D operations: `./uw3d (erode|dilate|open|close) image.p3m SE.p3m`


An additional program to generate image files to use as structuring element is provided:

`./se_gen (disk|rectangle|h|checkerboard|noise) diameter`

### MATLAB

`erode(f,SE)`, `dilate(f,SE)`, `open(f,SE)` and `close(f,SE)`.

**Supports uint8 only**

<sup>1. Erik R. Urbach, Michael H. F. Wilkinson, "Efficient 2-D Grayscale Morphological Transformations With Arbitrary Flat Structuring Elements", Image Processing IEEE Transactions on, vol. 17, pp. 1-8, 2008, ISSN 1057-7149. IEEE. https://doi.org/10.1109/TIP.2007.912582</sup>
