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


