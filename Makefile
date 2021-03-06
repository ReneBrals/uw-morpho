CC=g++
CFLAGS=-Wall -g -lm -O3 -fopenmp -mtune=native

all: 2d_sse2 3d_sse2 bench se_gen
fast: 2d_avx2 3d_avx2 benchfast se_gen
slow: 2d 3d benchslow se_gen

2d_avx2: src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mavx2 src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o uw2d

3d_avx2: src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mavx2 src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c -o uw3d

2d_sse2: src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -msse2 src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o uw2d

3d_sse2: src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -msse2 src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c -o uw3d

2d: src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mno-sse2 src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o uw2d

3d: src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mno-sse2 src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c src/safeMalloc.c -o uw3d

bench: src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -msse2 src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o bench
benchfast: src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mavx2 src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o bench
benchslow: src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mno-sse2 src/benchmain.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c src/safeMalloc.c -o bench

split: src/main.c src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -msse2 src/main.c src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c -o uw2d
splitfast: src/main.c src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -mavx2 src/main.c src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c -o uw2d
splitslow: src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c
	$(CC) $(CFLAGS) -no-sse2 src/main.c src/transformSplitting.c src/chordSet.c src/imagePGM.c src/LUTSplitting.c src/SIMD.c src/safeMalloc.c -o uw2d

se_gen: src/seGenerateMain.c src/imagePGM.c src/safeMalloc.c
	$(CC) $(CFLAGS) src/seGenerateMain.c src/imagePGM.c src/safeMalloc.c -o se_gen
