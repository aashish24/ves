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

#include "vesFBORenderTarget.h"

// VES includes
#include "vesFBO.h"
#include "vesTexture.h"

// C++ includes
#include <map>

vesFBORenderTarget::vesFBORenderTarget() : vesRenderTarget()
{
  this->m_implementation = FrameBufferObject;
  this->m_fbo = new vesFBO();
}


vesFBORenderTarget::~vesFBORenderTarget()
{
  delete this->m_fbo; this->m_fbo = 0x0;
}


bool vesFBORenderTarget::attach(vesFBO::AttachmentType type, vesTexture *texture)
{
  return this->m_fbo->setTexture(type, texture);
}


