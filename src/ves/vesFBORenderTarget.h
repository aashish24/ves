#ifndef VESFBORENDERTARGET_H
#define VESFBORENDERTARGET_H

#include "vesRenderTarget.h"

// Forward declarations
class vesTexture;

class vesFBORenderTarget : public vesRenderTarget
{
public:

           vesFBORenderTarget();
  virtual ~vesFBORenderTarget();

  virtual void attach(AttachmentType type, vesTexture *texture);
  virtual void render(vesRenderState &renderState);


protected:

  class vesInternal;
  vesInternal *m_internal;
};

#endif // VESFBORENDERTARGET_H
