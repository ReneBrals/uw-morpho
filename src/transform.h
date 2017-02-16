#ifndef _UWTRANSFORM
#define _UWTRANSFORM

#include "chordSet.h"
#include "imagePGM.h"

void dilate(image* g, image f, chordSet SE);
void erode(image* g, image f, chordSet SE);
void open(image* g, image f, chordSet SE);
void close(image* g, image f, chordSet SE);
void whiteTopHat(image* g, image f, chordSet SE);
void blackTopHat(image* g, image f, chordSet SE);

#endif
