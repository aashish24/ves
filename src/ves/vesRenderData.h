#ifndef VESRENDERDATA_H
#define VESRENDERDATA_H

#include "vesGLTypes.h"

struct vesRenderData
{
  vesRenderData(int primitiveType) :
    m_pritimiveType(primitiveType)
  {
  }

  int m_pritimiveType;
};

#endif // VESRENDERDATA_H
