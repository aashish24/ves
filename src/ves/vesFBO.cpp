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

#include "vesFBO.h"

// VES includes
#include "vesTexture.h"

// C/C++ includes
#include <algorithm>
#include <iostream>
#include <map>

class vesFBO::vesInternal
{
public:
  vesInternal() :
    m_frameBufferHandle(0),
    m_width            (0),
    m_height           (0)
  {
  }


 ~vesInternal()
  {
    this->cleanUp();
  }


  void cleanUp()
  {
    this->m_attachmentToTextureMap.clear();
    this->m_attachmentToRBOMap.clear();
  }


  void validateAndFixTextureDimensions(vesTexture *texture)
  {
    if (!texture) {
      return;
    }

    if (texture->width() != this->m_width) {
      texture->setWidth(this->m_width);
    }

    if (texture->height() != this->m_height) {
      texture->setHeight(this->m_height);
    }
  }


  typedef std::map<AttachmentType, vesTexture*>  AttachmentToTextureMap;
  typedef std::map<AttachmentType, unsigned int> AttachmentToRBOMap;


  unsigned int m_frameBufferHandle;

  int m_width;
  int m_height;

  AttachmentToTextureMap m_attachmentToTextureMap;
  AttachmentToRBOMap     m_attachmentToRBOMap;
};


vesFBO::vesFBO() : vesObject()
{
  this->m_internal = new vesInternal();

  this->setDirtyStateOn();
}


vesFBO::~vesFBO()
{
  delete this->m_internal; this->m_internal = 0x0;
}


bool vesFBO::setTexture(AttachmentType type, vesTexture *texture)
{
  if (!texture) {
    return false;
  }

  vesInternal::AttachmentToTextureMap::iterator itr =
    this->m_internal->m_attachmentToTextureMap.find(type);

  if (itr != this->m_internal->m_attachmentToTextureMap.end()) {
    if (texture == itr->second) {
      return false;
    }
    else {
      this->m_internal->m_attachmentToTextureMap[type] = texture;
      this->setDirtyStateOn();
      return true;
    }
  }
  else {
    this->m_internal->m_attachmentToTextureMap[type] = texture;
    this->setDirtyStateOn();
    return true;
  }
}


vesTexture* vesFBO::texture(AttachmentType type)
{
  vesInternal::AttachmentToTextureMap::iterator itr =
    this->m_internal->m_attachmentToTextureMap.find(type);

  if (itr != this->m_internal->m_attachmentToTextureMap.end()) {
    return itr->second;
  }

  return 0x0;
}


const vesTexture* vesFBO::texture(AttachmentType type) const
{
  vesInternal::AttachmentToTextureMap::iterator itr =
    this->m_internal->m_attachmentToTextureMap.find(type);

  if (itr != this->m_internal->m_attachmentToTextureMap.end()) {
    return itr->second;
  }

  return 0x0;
}


void vesFBO::setWidth(int width)
{
  this->m_internal->m_width = width;

  this->setDirtyStateOn();
}


int vesFBO::width() const
{
  return this->m_internal->m_width;
}


void vesFBO::setHeight(int height)
{
  this->m_internal->m_height = height;

  this->setDirtyStateOn();
}


int vesFBO::height() const
{
  return this->m_internal->m_height;
}


void vesFBO::setup(vesRenderState &renderState)
{
  if (this->m_dirtyState) {
    // Delete all from past.
    this->deleteFBO(renderState);

    this->createFBO(renderState);
  }
}


void vesFBO::render(vesRenderState &renderState)
{
  // Call setup in case we have not done so already.
  this->setup(renderState);

  // Check for framebuffer complete
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status == GL_FRAMEBUFFER_COMPLETE) {
    glBindFramebuffer(GL_FRAMEBUFFER, this->m_internal->m_frameBufferHandle);
  }
  else {
    switch(status) {
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT " << status << std::endl;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS " << status << std::endl;
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      std::cerr << "GL ERROR: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT " << status << std::endl;
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
      std::cerr << "GL ERROR: GL_FRAMEBUFFER_UNSUPPORTED " << status << std::endl;
      break;
    default:
      std::cerr << "GL ERROR: Unknown error " << status << std::endl;
      break;
    };
  }
}


void vesFBO::remove(vesRenderState &renderState)
{
  vesNotUsed(renderState);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void vesFBO::createFBO(vesRenderState &renderState)
{
  glGenFramebuffers(1, &this->m_internal->m_frameBufferHandle);
  glBindFramebuffer(GL_FRAMEBUFFER, this->m_internal->m_frameBufferHandle);

  vesInternal::AttachmentToTextureMap::iterator itr =
    this->m_internal->m_attachmentToTextureMap.find(ColorAttachment0);

  if(itr == this->m_internal->m_attachmentToTextureMap.end()) {
    unsigned int colorBufferHandle;
    glGenRenderbuffers(1, &colorBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, colorBufferHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGB565,
                          this->m_internal->m_width,
                          this->m_internal->m_height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, ColorAttachment0,
                              GL_RENDERBUFFER, colorBufferHandle);
    this->m_internal->m_attachmentToRBOMap[ColorAttachment0] = colorBufferHandle;
  }
  else {
    this->m_internal->validateAndFixTextureDimensions(itr->second);
    itr->second->setup(renderState);
    glFramebufferTexture2D(GL_FRAMEBUFFER, ColorAttachment0, GL_TEXTURE_2D,
      this->m_internal->m_attachmentToTextureMap[ColorAttachment0]->textureHandle(), 0);
  }

  itr = this->m_internal->m_attachmentToTextureMap.find(DepthAttachment);
  if(itr == this->m_internal->m_attachmentToTextureMap.end()) {
    unsigned int depthBufferHandle;
    glGenRenderbuffers(1, &depthBufferHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBufferHandle);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16,
                          this->m_internal->m_width,
                          this->m_internal->m_height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthBufferHandle);
    this->m_internal->m_attachmentToRBOMap[DepthAttachment] = depthBufferHandle;
  }
  else {
    this->m_internal->validateAndFixTextureDimensions(itr->second);
    glFramebufferTexture2D(GL_FRAMEBUFFER, DepthAttachment, GL_TEXTURE_2D,
      this->m_internal->m_attachmentToTextureMap[DepthAttachment]->textureHandle(), 0);
  }

  this->setDirtyStateOff();
}


void vesFBO::deleteFBO(vesRenderState &renderState)
{
  this->remove(renderState);

  vesInternal::AttachmentToRBOMap::iterator itr = this->m_internal->m_attachmentToRBOMap.end();

  for (; itr != this->m_internal->m_attachmentToRBOMap.end(); ++itr) {
    glDeleteRenderbuffers(1, &(itr->second));
  }

  glDeleteFramebuffers (1, &this->m_internal->m_frameBufferHandle);
}
