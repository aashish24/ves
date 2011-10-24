#ifndef VESBACKGROUND_H
#define VESBACKGROUND_H

#include "vesCamera.h"

// Forward declarations.
class vesActor;
class vesDepth;
class vesMapper;

class vesBackground : public vesCamera
{
public:
  vesBackground();
  virtual ~vesBackground();

  virtual void setGradientColor(const vesVector4f &topColor,
                                const vesVector4f &bottomColor);
  virtual vesMatrix4x4f modelViewMatrix();
  virtual vesMatrix4x4f projectionMatrix();

protected:
  void createBackground();

  vesVector4f m_topColor;
  vesVector4f m_bottomColor;

  class vesInternal;
  vesInternal *m_internal;
};

#endif // VESBACKGROUND_H
