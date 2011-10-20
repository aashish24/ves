#ifndef VESCOLORDATATYPE_H
#define VESCOLORDATATYPE_H

#include "vesGL.h"

struct vesColorDataType
{
  enum PixelFormat
  {
    None            = 0,
    Alpha           = GL_ALPHA,
    Luminance       = GL_LUMINANCE,
    LuminanceAlpha  = GL_LUMINANCE_ALPHA,
    RGB             = GL_RGB,
    RGBA            = GL_RGBA
  };
};


#endif // VESCOLORDATATYPE_H
