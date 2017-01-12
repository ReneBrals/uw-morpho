#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "transform.h"
#include "chordSet.h"
#include "imagePGM.h"
#include "LUT.h"

int main(int argc, char* argv[]){
	if(argc < 4){
		printf("usage: uw (erode|dilate|open|close|whitetophat|blacktophat) image.pgm se.pbm\n");
		return 0;
	}

	image SEimg = readPGM(argv[3], 0, 0);

	chordSet SE = buildChordSet(SEimg);
	freeImage(SEimg);
    image g, f;

	f = readPGM(argv[2], 0 - SE.minY, SE.maxY>0?SE.maxY:0);

    if(!strcmp(argv[1],"erode")){
        g = erode(f,SE);
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
