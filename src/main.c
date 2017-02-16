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

	g = initImage(f.W, f.H, f.range);

    if(!strcmp(argv[1],"erode")){
		erode(&g, f, SE);
    } else if(!strcmp(argv[1],"dilate")){
        dilate(&g, f, SE);
    } else if(!strcmp(argv[1],"open")){
		open(&g, f, SE);
	} else if(!strcmp(argv[1],"close")){
		close(&g, f, SE);
	} else if(!strcmp(argv[1],"whitetophat")){
		close(&g, f, SE);
	} else if(!strcmp(argv[1],"blacktophat")){
		close(&g, f, SE);
	} else {
        printf("ERROR: operation '%s' not supported",argv[1]);
    }

	freeImage(f);

	writePGM("out.pgm",g);

	freeChordSet(SE);

	freeImage(g);

	return 0;
}
