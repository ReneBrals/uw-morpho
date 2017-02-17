#include <stdlib.h>
#include <string.h>
#include "imagePGM.h"

int main(int argc, char* argv[]){
	if(argc < 3){
		printf("usage: se_gen (disk|rectangle|h|checkerboard|noise) size\n");
		return 0;
	}

	int d = atoi(argv[2]);

    if(!strcmp(argv[1], "disk")){
		writePGM("disk.pgm", disk(d));
    } else if(!strcmp(argv[1], "rectangle")){
		writePGM("rectangle.pgm", rectangle(d));
    } else if(!strcmp(argv[1], "h")){
		writePGM("h.pgm", h(d));
	} else if(!strcmp(argv[1], "checkerboard")){
		writePGM("checkerboard.pgm", checkerboard(d));
	} else if(!strcmp(argv[1], "noise")){
		writePGM("noise.pgm", noise(d));
	} else {
        printf("ERROR: shape '%s' not supported", argv[1]);
    }

	return 0;
}
