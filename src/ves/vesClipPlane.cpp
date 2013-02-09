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

#include "vesClipPlane.h"

// VES includes.
#include "vesGL.h"

#include <cassert>
#include <iostream>

vesClipPlane::vesClipPlane() : vesMaterialAttribute()
#if 0
  ,m_wasEnabled(false)
#endif
{
  this->m_type = vesMaterialAttribute::ClipPlane;
  this->m_binding = BindMinimal;

  this->m_planeNumber = 0;

  this->m_planeEquation[0] = 0.0;
  this->m_planeEquation[1] = 0.0;
  this->m_planeEquation[2] = 0.0;
  this->m_planeEquation[3] = 0.0;
}


vesClipPlane::~vesClipPlane()
{
}


void vesClipPlane::setPlaneEquation(float a, float b, float c, float d)
{
  this->m_planeEquation[0] = a;
  this->m_planeEquation[1] = b;
  this->m_planeEquation[2] = c;
  this->m_planeEquation[3] = d;

  this->setDirtyStateOn();
}

void vesClipPlane::getPlaneEquation(float &a, float &b, float &c, float &d)
{
  a = this->m_planeEquation[0];
  b = this->m_planeEquation[1];
  c = this->m_planeEquation[2];
  d = this->m_planeEquation[3];
}


void vesClipPlane::setPlaneNumber(int i)
{
  assert(i >= 0);

  if (this->m_planeNumber != i && i >= 0) {
    this->m_planeNumber = i;
    this->setDirtyStateOn();
  }
  else if  (i < 0){
    std::cerr << "Plane number should be 0 or higher" << std::endl;
  }

  return;
}


int vesClipPlane::getPlaneNumber() {
  return this->m_planeNumber;
}


void vesClipPlane::bind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

#if 0 // ES 1.0 does not provide glIsEnabled
  // Save current state.
  this->m_wasEnabled = glIsEnabled(GL_CLIP_PLANE0 + this->m_planeNumber);
#endif

#if 0
  if (this->m_enable) {
    GLfloat eqn2[4] = {1.0, 0.0, 1.0, 0.0};
    glEnable(GL_CLIP_PLANE0 + this->m_planeNumber);
    glClipPlanexOES(GL_CLIP_PLANE0, eqn2);
  } else {
    glDisable(GL_CLIP_PLANE0 + this->m_planeNumber);
  }
#endif
}


void vesClipPlane::unbind(const vesRenderState &renderState)
{
  vesNotUsed(renderState);

#if 0 // ES 1.0 does not provide glIsEnabled
  if (this->m_wasEnabled) {
      glEnable(GL_CLIP_PLANE0 + this->m_planeNumber);
  } else {
    glDisable(GL_CLIP_PLANE0 + this->m_planeNumber);
  }
#endif

  this->setDirtyStateOff();
}
