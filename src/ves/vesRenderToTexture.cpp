/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include "vesRenderToTexture.h"

// VES includes
#include "vesFBORenderTargetPrivate.h"
#include "vesTexture.h"

// C/C++ includes
#include <cassert>
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

  this->setDirtyStateOn();

  return this->attach(ColorAttachment0, texture);
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

    vesFBORenderTarget::vesInternal::BufferAttachmentMap::iterator itr =
      this->m_internal->m_bufferAttachmentMap.find(ColorAttachment0);

    if (itr != this->m_internal->m_bufferAttachmentMap.end()) {

      itr->second.m_texture->setup(renderState);

      glBindTexture(GL_TEXTURE_2D, itr->second.m_texture->textureHandle());

      unsigned int renderBufferHandle;
      glGenRenderbuffers(1, &renderBufferHandle);
      glBindRenderbuffer(GL_RENDERBUFFER, renderBufferHandle);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                            itr->second.m_texture->width(),
                            itr->second.m_texture->height());

      this->m_internal->m_renderBuffersHandle.push_back(renderBufferHandle);

      glBindFramebuffer(GL_FRAMEBUFFER, this->m_internal->m_frameBufferHandle);

      // Specify texture as color attachment
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                             itr->second.m_texture->textureHandle(), 0);

      // Specify depth_renderbufer as depth attachment
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, renderBufferHandle);
    }

    this->setDirtyStateOff();
  }
}


void vesRenderToTexture::render(vesRenderState &renderState)
{
  assert(this->m_internal->m_renderBuffersHandle.empty());

  // Call setup in case we have not done so already.
  this->setup(renderState);

  // Check for framebuffer complete
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if(status == GL_FRAMEBUFFER_COMPLETE)
  {
    glClearColor(1.0, 0.0, 0.0, 1.0);
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_internal->m_frameBufferHandle);
  }
  else
  {
    switch(status)
    {
      case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      {
        std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT " << status << std::endl;
        break;
      }
      case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      {
        std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS " << status << std::endl;
        break;
      }
      case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      {
        std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT " << status << std::endl;
        break;
      }
      case GL_FRAMEBUFFER_UNSUPPORTED:
      {
        std::cerr << "GL ERROR: GL_FRAMEBUFFER_UNSUPPORTED " << status << std::endl;
        break;
      }
      default:
      {
        std::cerr << "GL ERROR: Unknown error " << status << std::endl;
      }
    };
  }
}


void vesRenderToTexture::remove(vesRenderState &renderState)
{
  glDeleteRenderbuffers(1, &this->m_internal->m_renderBuffersHandle[0]);
  glDeleteFramebuffers (1, &this->m_internal->m_frameBufferHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  this->m_internal->m_renderBuffersHandle.clear();
}

