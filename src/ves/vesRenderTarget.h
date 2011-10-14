#ifndef VESRENDERTARGET_H
#define VESRENDERTARGET_H

#include "vesObject.h"

// VES includes
#include "vesGL.h"

// Forward declarations
class vesRenderState;

class vesRenderTarget : public vesObject
{
public:
  vesRenderTarget() :
    m_implementation(FrameBuffer)
  {
  }


  enum ImplementationType
  {
      // Default
      FrameBuffer = 0,
      FrameBufferObject,
      PixelBuffer
  };


  enum AttachmentType
  {
    ColorAttachment0 = GL_COLOR_ATTACHMENT0,
    DepthAttachment  = GL_DEPTH_ATTACHMENT
  };

  virtual ~vesRenderTarget(){}

  virtual void setup(vesRenderState &renderState){}
  virtual void render(vesRenderState &renderState){}


 protected:

   ImplementationType m_implementation;
};


#endif // VESRENDERTARGET_H
