/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#ifndef VESCOLORDATATYPE_H
#define VESCOLORDATATYPE_H

// VES includes
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
