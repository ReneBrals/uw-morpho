# uw-morpho
Implementation of the Urbach-Wilkinson algorithm for grayscale morphological operations with arbitrary flat structuring elements. Will support 2D and 3D images.

Makes use of vector intrinsics to increase performance. Compile with ```make fast``` for AVX2 goodness (requires an Intel Haswell processor or newer), simply ```make``` for SSE2 (requires Intel Pentium 4 or newer), or ```make slow``` if these instructions are not available.
