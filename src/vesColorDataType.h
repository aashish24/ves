#ifndef VESCOLORDATATYPE_H
#define VESCOLORDATATYPE_H

#include "vesGL.h"

struct vesColorDataType
{
  enum PixelFormat
  {
    PixelFormatNone = 0,
    Alpha           = GL_ALPHA,
    Luminance       = GL_LUMINANCE,
    LuminanceAlpha  = GL_LUMINANCE_ALPHA,
    RGB             = GL_RGB,
    RGBA            = GL_RGBA
  };

  enum PixelDataType
  {
    PixelDataTypeNone = 0,
    UnsignedByte      = GL_UNSIGNED_BYTE,
    UnsignedShort565  = GL_UNSIGNED_SHORT_5_6_5,
    UnsignedShort4444 = GL_UNSIGNED_SHORT_4_4_4_4,
    UnsignedShort5551 = GL_UNSIGNED_SHORT_5_5_5_1
  };
};


#endif // VESCOLORDATATYPE_H
