
#include "vesFBORenderTarget.h"

vesRenderTarget::vesRenderTarget() : vesSceneObject()
{
  this->m_implementation = FrameBufferObject;
}


~vesRenderTarget::vesFBORenderTarget()
{
}

