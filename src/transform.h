#ifndef _UWTRANSFORM
#define _UWTRANSFORM

#include "chordSet.h"
#include "imagePGM.h"

#define ERODE 0
#define DILATE 1

image dilate(image f, chordSet SE);
image erode(image f, chordSet SE);
image open(image f, chordSet SE);
image close(image f, chordSet SE);
image whiteTopHat(image f, chordSet SE);
image blackTopHat(image f, chordSet SE);
#endif
