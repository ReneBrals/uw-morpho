# uw-morpho
Implementation of the Urbach-Wilkinson<sup>1</sup> algorithm for grayscale morphological operations with arbitrary flat structuring elements. Will support 2D and 3D images.

Makes use of vector intrinsics to increase performance. Compile with ```make fast``` for AVX2 goodness (requires an Intel Haswell processor or newer), simply ```make``` for SSE2 (requires Intel Pentium 4 or newer), or ```make slow``` if these instructions are not available.

<sup>1. Urbach, E., & Wilkinson, M. (2006). Efficient 2-D Gray-Scale Dilations and Erosions with Arbitrary Flat Structuring Elements. In 2006 International Conference on Image Processing (Vol. 17, pp. 1573–1576). IEEE. http://doi.org/10.1109/ICIP.2006.312608</sup>
