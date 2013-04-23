
#include "vesLight.h"
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
#include "vesRenderState.h"

// C/C++ includes.
#include <iostream>

vesLight::vesLight() : vesMaterialAttribute(),
  m_wasEnabled(false),
  m_wasColorMaterialEnabled(false)
{
  this->m_type = Light;
  this->m_binding = BindMinimal;
}


vesLight::~vesLight()
{
}


void vesLight::bind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  // Save current state.
  // TODO Got undefined error for QNX
  this->m_wasEnabled = renderState.getGlobalRenderState().isEnabled(
    GL_LIGHTING);
  this->m_wasColorMaterialEnabled = renderState.getGlobalRenderState().isEnabled(
    GL_COLOR_MATERIAL);

  if (this->m_enable) {
     renderState.getGlobalRenderState().enable(GL_LIGHTING);
  } else {
    renderState.getGlobalRenderState().disable(GL_LIGHTING);
    renderState.getGlobalRenderState().disable(GL_COLOR_MATERIAL);
  }
}


void vesLight::unbind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

  if (this->m_wasEnabled) {
    renderState.getGlobalRenderState().enable(GL_LIGHTING);
  } else {
    renderState.getGlobalRenderState().disable(GL_LIGHTING);
  }

  if (this->m_wasColorMaterialEnabled && !this->m_enable) {
    renderState.getGlobalRenderState().enable(GL_COLOR_MATERIAL);
  }

  this->setDirtyStateOff();
}
