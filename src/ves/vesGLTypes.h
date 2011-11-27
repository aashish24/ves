#ifndef VESGLTYPES_H
#define VESGLTYPES_H

#include "vesGL.h"

struct vesPrimitiveRenderType
{
  enum Type
  {
    Points        = GL_POINTS,
    LineStrip     = GL_LINE_STRIP,
    LineLoop      = GL_LINE_LOOP,
    Lines         = GL_LINES,
    TriangleStrip = GL_TRIANGLE_STRIP,
    TriangleFan   = GL_TRIANGLE_FAN,
    Triangles     = GL_TRIANGLES
  };
};


#endif // VESGLTYPES_H
