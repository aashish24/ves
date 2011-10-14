
#include "vesFBORenderTarget.h"

// VES includes
#include "vesFBORenderTargetPrivate.h"
#include "vesTexture.h"

// C++ includes
#include <map>

vesFBORenderTarget::vesFBORenderTarget() : vesRenderTarget()
{
  this->m_internal = new vesInternal();

  this->m_implementation = FrameBufferObject;
}


vesFBORenderTarget::~vesFBORenderTarget()
{
  delete this->m_internal;
}


bool vesFBORenderTarget::attach(AttachmentType type, vesTexture *texture)
{
  if (texture) {
    this->m_internal->m_bufferAttachmentMap[type].m_texture = texture;

    return true;
  }

  return false;
}


