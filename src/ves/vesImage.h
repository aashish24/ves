#ifndef VESIMAGE_H
#define VESIMAGE_H

#include "vesColorDataType.h"

struct vesImage
{
  vesImage() :
    m_width(0),
    m_height(0),
    m_depth(0),
    m_pixelFormat(vesColorDataType::None),
    m_data(0x0)
  {
  }

  int m_width;
  int m_height;
  int m_depth;

  vesColorDataType::PixelFormat m_pixelFormat;

  void *m_data;
};

#endif // VESIMAGE_H
