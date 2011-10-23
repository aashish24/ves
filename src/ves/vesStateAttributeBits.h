#ifndef VESSTATEATTRIBUTEBITS_H
#define VESSTATEATTRIBUTEBITS_H

// VES includes.
#include "vesGL.h"

class vesStateAttributeBits
{
public:
  enum ClearMask
  {
    ColorBufferBit = GL_COLOR_BUFFER_BIT,
    DepthBufferBit = GL_DEPTH_BUFFER_BIT
  };
};

#endif // VESSTATEATTRIBUTEBITS_H
