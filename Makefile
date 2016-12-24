erode: erode.c chordSet.c imagePGM.c LUT.c simdMIN.c
	g++ -o erode erode.c chordSet.c imagePGM.c LUT.c simdMIN.c -Wall -g -lm -O3 -mavx2 -mtune=native
