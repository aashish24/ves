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

#include "vesViewport.h"

// VES includes.
#include "vesGL.h"

vesViewport::vesViewport()
{
  this->m_x = 0;
  this->m_y = 0;
  this->m_width = 800;
  this->m_height = 600;
}


vesViewport::vesViewport(int x, int y, int width, int height) :
  m_x(x),
  m_y(y),
  m_width(width),
  m_height(height)
{
}


vesViewport::~vesViewport()
{
}


void vesViewport::render(vesRenderState &renderState)
{
  glViewport(static_cast<GLint>(this->m_x), static_cast<GLint>(this->m_y),
             static_cast<GLsizei>(this->m_width), static_cast<GLsizei>(this->m_height));
}


