CC=g++
CFLAGS=-Wall -g -lm -O3 -mavx2 -mtune=native -fopenmp

all: 2d 3d

2d: src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c
	$(CC) $(CFLAGS) src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c  -o uw2d

3d: src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c
	$(CC) $(CFLAGS) src/main3D.c src/transform3D.c src/chordSet3D.c src/imageP3M.c src/LUT3D.c src/SIMD.c -o uw3d
