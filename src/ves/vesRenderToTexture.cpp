
#include "vesRenderToTexture.h"

// VES includes
#include "vesFBORenderTargetPrivate.h"
#include "vesTexture.h"

// C++ includes
#include <map>

vesRenderToTexture::vesRenderToTexture() : vesFBORenderTarget()
{
}


vesRenderToTexture::~vesRenderToTexture()
{
}


bool vesRenderToTexture::setTexture(vesTexture *texture)
{
  if (!texture) {
    return false;
  }

  std::map<AttachmentType, Attachment>::iterator itr;
  itr = this->m_internal->m_bufferAttachmentMap.find(ColorAttachment0);
  if (itr != this->m_internal->m_bufferAttachmentMap.end() &&
      itr->second.m_texture == texture) {
    return false;
  }

  this->attach(ColorAttachment0, texture);
}


vesTexture* vesRenderToTexture::texture()
{
  std::map<AttachmentType, Attachment>::iterator itr;
  itr = this->m_internal->m_bufferAttachmentMap.find(ColorAttachment0);
  if (itr != this->m_internal->m_bufferAttachmentMap.end()) {
    return itr->second.m_texture;
  }
  else {
    return 0x0;
  }
}


const vesTexture* vesRenderToTexture::texture() const
{
  std::map<AttachmentType, Attachment>::const_iterator constItr;
  constItr = this->m_internal->m_bufferAttachmentMap.find(ColorAttachment0);
  if (constItr != this->m_internal->m_bufferAttachmentMap.end()) {
    return constItr->second.m_texture;
  }
  else {
    return 0x0;
  }
}


void vesRenderToTexture::setup(vesRenderState &renderState)
{
  if (this->m_dirtyState) {
    glGenFramebuffers(1, &this->m_internal->m_frameBufferHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_internal->m_frameBufferHandle);

    vesFBORenderTarget::vesInternal::BufferAttachmentMap::iterator itr =
      this->m_internal->m_bufferAttachmentMap.find(ColorAttachment0);

    if (itr != this->m_internal->m_bufferAttachmentMap.end()) {
      unsigned int renderBufferHandle;
      glGenRenderbuffers(1, &renderBufferHandle);
      glBindRenderbuffer(GL_RENDERBUFFER, renderBufferHandle);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
        itr->second.m_texture->width(), itr->second.m_texture->height());

      this->m_internal->m_renderBuffersHandle.push_back(renderBufferHandle);

      glBindTexture(GL_TEXTURE_2D, itr->second.m_texture->textureHandle());

      // Specify texture as color attachment
      glFramebufferTexture2D(GL_FRAMEBUFFER, itr->first, GL_TEXTURE_2D,
                             itr->second.m_texture->textureHandle(), 0);

      // Specify depth_renderbufer as depth attachment
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, renderBufferHandle);
    }

    this->setDirtyStateOff();
  }
}


void vesFBORenderTarget::render(vesRenderState &renderState)
{
  glBindRenderbuffer(GL_RENDERBUFFER, this->m_internal->m_frameBufferHandle);

  vesFBORenderTarget::vesInternal::BufferAttachmentMap::iterator itr =
    this->m_internal->m_bufferAttachmentMap.begin();

  glBindRenderbuffer(GL_RENDERBUFFER, this->m_internal->m_renderBuffersHandle.back());

  glBindTexture(GL_TEXTURE_2D, itr->second.m_texture->textureHandle());
}





