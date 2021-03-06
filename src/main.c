#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "transform.h"
#include "chordSet.h"
#include "imagePGM.h"
#include "LUT.h"

int main(int argc, char* argv[]){
	if(argc < 4){
		printf("usage: uw (erode|dilate|open|close|whitetophat|blacktophat) image_file se_file\n");
		return 0;
	}

	image SEimg = readImage(argv[3]);

	chordSet SE = buildChordSet(SEimg);
	freeImage(SEimg);
    image g, f;

	f = readImage(argv[2]);

	g = initImage(f.W, f.H, f.range);

    if(!strcmp(argv[1], "erode")){
		erode(&g, f, SE);
		writePGM("out.pgm", g);
    } else if(!strcmp(argv[1], "dilate")){
        dilate(&g, f, SE);
		writePGM("out.pgm", g);
    } else if(!strcmp(argv[1], "open")){
		open(&g, f, SE);
		writePGM("out.pgm", g);
	} else if(!strcmp(argv[1], "close")){
		close(&g, f, SE);
		writePGM("out.pgm", g);
	} else if(!strcmp(argv[1], "whitetophat")){
		whiteTopHat(&g, f, SE);
		writePGM("out.pgm", f);
	} else if(!strcmp(argv[1], "blacktophat")){
		blackTopHat(&g, f, SE);
		writePGM("out.pgm", g);
	} else {
        printf("ERROR: operation '%s' not supported", argv[1]);
    }

	freeImage(f);
	freeChordSet(SE);
	freeImage(g);

	return 0;
}
