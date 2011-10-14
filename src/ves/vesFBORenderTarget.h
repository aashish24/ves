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

  virtual bool attach(AttachmentType type, vesTexture *texture);

protected:

  class vesInternal;
  vesInternal *m_internal;
};

#endif // VESFBORENDERTARGET_H
