
#include "vesDepth.h"
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

// VES includes.
#include "vesGL.h"

// C/C++ includes.
#include <iostream>

vesDepth::vesDepth() : vesMaterialAttribute(),
  m_wasEnabled(false),
  m_previousDepthWriteMask(true),
  m_depthWriteMask(true)
{
  this->m_type = Depth;
  this->m_binding = BindMinimal;
}


vesDepth::~vesDepth()
{
}


void vesDepth::bind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  // Save current state.
  this->m_wasEnabled = glIsEnabled(GL_DEPTH_TEST);

  // Save current depth mask for restoration later.
//  glGet(GL_DEPTH_WRITEMASK, &this->m_previousDepthWriteMask);

  if (this->m_enable) {
    glEnable(GL_DEPTH_TEST);
    glDepthMask((GLboolean) this->m_depthWriteMask);
  } else {
    glDisable(GL_DEPTH_TEST);
  }
}


void vesDepth::unbind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  if (this->m_wasEnabled) {
      glEnable(GL_DEPTH_TEST);
      glDepthMask((GLboolean) this->m_depthWriteMask);
  } else {
    glDisable(GL_DEPTH_TEST);
  }

  // Restore previous depth mask.
//  glDepthMask(this->m_previousDepthWriteMask);

  this->setDirtyStateOff();
}
