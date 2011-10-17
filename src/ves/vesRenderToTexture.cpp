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
#include "vesFBO.h"
#include "vesGL.h"
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


bool vesRenderToTexture::setColorTexture(vesTexture *texture)
{
  bool result = this->m_fbo->setTexture(vesFBO::ColorAttachment0, texture);

  if (result) {
    this->setDirtyStateOn();
  }

  return result;
}


vesTexture* vesRenderToTexture::colorTexture()
{
  return this->m_fbo->texture(vesFBO::ColorAttachment0);
}


const vesTexture* vesRenderToTexture::colorTexture() const
{
  return this->m_fbo->texture(vesFBO::ColorAttachment0);
}


void vesRenderToTexture::setup(vesRenderState &renderState)
{
  if (this->m_dirtyState) {

    this->m_fbo->setup(renderState);

    this->setDirtyStateOff();
  }
}


void vesRenderToTexture::render(vesRenderState &renderState)
{
  this->m_fbo->render(renderState);
}


void vesRenderToTexture::remove(vesRenderState &renderState)
{
  this->m_fbo->remove(renderState);
}
