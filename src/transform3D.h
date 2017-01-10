#ifndef _UWTRANSFORM3
#define _UWTRANSFORM3

#include "chordSet3D.h"
#include "imageP3M.h"

#define ERODE 0
#define DILATE 1

image3D      dilate3D(image3D f, chordSet3D SE);
image3D       erode3D(image3D f, chordSet3D SE);
image3D        open3D(image3D f, chordSet3D SE);
image3D       close3D(image3D f, chordSet3D SE);
image3D whiteTopHat3D(image3D f, chordSet3D SE);
image3D blackTopHat3D(image3D f, chordSet3D SE);
#endif
