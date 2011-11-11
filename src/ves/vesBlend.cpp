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

#include "vesBlend.h"

// C/C++ includes
#include <iostream>

vesBlend::vesBlend() : vesMaterialAttribute(),
  m_wasEnabled(false),
  m_blendFunction(vesBlendFunction::SrcAlpha, vesBlendFunction::OneMinusSrcAlpha)
{
  this->m_type = Blend;
  this->m_binding = BindMinimal;
}


vesBlend::~vesBlend()
{
}


void vesBlend::setBlendFunction(const vesBlendFunction &blendFunction)
{
  this->m_blendFunction = blendFunction;
  this->setDirtyStateOn();
}


void vesBlend::bind(const vesRenderState &renderState)
{
  this->m_wasEnabled = glIsEnabled(GL_BLEND);

  if (this->m_enable) {
    glEnable(GL_BLEND);
    this->m_blendFunction.apply(renderState);
  } else {
    glDisable(GL_BLEND);
  }
}


void vesBlend::unbind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  if (this->m_wasEnabled) {
    glEnable(GL_BLEND);
  } else {
    glDisable(GL_BLEND);
  }

  this->setDirtyStateOff();
}


