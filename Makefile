all: src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c
	g++ -o uw src/main.c src/transform.c src/chordSet.c src/imagePGM.c src/LUT.c src/SIMD.c -Wall -g -lm -O3 -mavx2 -mtune=native
