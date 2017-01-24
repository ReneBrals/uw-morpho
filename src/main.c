#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "transform.h"
#include "chordSet.h"
#include "imagePGM.h"
#include "LUT.h"
#include <omp.h>

int main(int argc, char* argv[]){
	if(argc < 4){
		printf("usage: uw (erode|dilate|open|close|whitetophat|blacktophat) image.pgm se.pbm\n");
		return 0;
	}

	image SEimg = readPGM(argv[3]);

	chordSet SE = buildChordSet(SEimg);
	freeImage(SEimg);
    image g, f;

	f = readPGM(argv[2]);

    if(!strcmp(argv[1],"erode")){
		g = erode(f,SE);
		/*
		double tick, tock;
		printf("multi: ");
		for(int i = 0; i<10; i++){
			tick = omp_get_wtime();
			g = erode(f,SE);
			tock = omp_get_wtime();
			freeImage(g);
			printf("%2.3f\t",tock-tick);
		}
		printf("\n");
		*/
    } else if(!strcmp(argv[1],"dilate")){
        g = dilate(f,SE);
    } else if(!strcmp(argv[1],"open")){
		g = open(f,SE);
	} else if(!strcmp(argv[1],"close")){
		g = close(f,SE);
	} else if(!strcmp(argv[1],"whitetophat")){
		g = close(f,SE);
	} else if(!strcmp(argv[1],"blacktophat")){
		g = close(f,SE);
	} else {
        printf("ERROR: operation '%s' not supported",argv[1]);
    }

	freeImage(f);

	writePGM("out.pgm",g);

	freeChordSet(SE);

	freeImage(g);

	return 0;
}
