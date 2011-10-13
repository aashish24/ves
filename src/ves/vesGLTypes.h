#ifndef VESGLTYPES_H
#define VESGLTYPES_H

#include "vesGL.h"

struct vesGLTypes
{
  enum vesPrimitiveTypes
    {
    Points        = 0,
    LineStrip     = 1,
    LineLoop      = 2,
    Lines         = 3,
    TriangleStrip = 4,
    TriangleFan   = 5,
    Triangles     = 6
    };
};


#endif // VESGLTYPES_H
