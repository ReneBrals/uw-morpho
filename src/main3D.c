#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "transform3D.h"
#include "chordSet3D.h"
#include "imageP3M.h"
#include "LUT3D.h"

int main(int argc, char* argv[]){
	if(argc < 4){
		printf("usage: uw3d (erode|dilate|open|close|whitetophat|blacktophat) image.p3m se.p3m\n");
		return 0;
	}

	image3D SEimg = readP3M(argv[3]);

	chordSet3D SE = buildChordSet3D(SEimg);
	freeImage3D(SEimg);
    image3D g, f;

	f = readP3M(argv[2]);

    if(!strcmp(argv[1],"erode")){
        g = erode3D(f,SE);
    } else if(!strcmp(argv[1],"dilate")){
        g = dilate3D(f,SE);
    } else if(!strcmp(argv[1],"open")){
		g = open3D(f,SE);
	} else if(!strcmp(argv[1],"close")){
		g = close3D(f,SE);
	} else if(!strcmp(argv[1],"whitetophat")){
		g = close3D(f,SE);
	} else if(!strcmp(argv[1],"blacktophat")){
		g = close3D(f,SE);
	} else {
        printf("ERROR: operation '%s' not supported",argv[1]);
    }

	freeImage3D(f);

	writeP3M("out.pgm",g);

	freeChordSet3D(SE);

	freeImage3D(g);

	return 0;
}
