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

#include "vesMapper.h"

#include "vesMaterial.h"
#include "vesRenderStage.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

vesMapper::vesMapper()
{
  this->m_initialized = false;
  this->m_data        = false;
}


vesMapper::~vesMapper()
{
  if (this->m_initialized) {
  }
}


void vesMapper::computeBounds()
{
  vesVector3f min = this->m_data->GetMin();
  vesVector3f max = this->m_data->GetMax();

  set_BBoxSize  (min, max);
  set_BBoxCenter(min, max);
}


void vesMapper::accept(vesVisitor &visitor)
{
}


void vesMapper::setData(vesTriangleData *data)
{
  if (data) {
    this->m_data = data;
  }
}


void vesMapper::normalize()
{
  float r = GetBBoxRadius();
  this->m_normalizedMatrix =
      makeScaleMatrix4x4(1/r,1/r,1/r)*
      makeTranslationMatrix4x4(-get_BBoxCenter());
  set_BBoxCenter(transformPoint3f(this->m_normalizedMatrix, get_BBoxCenter()));
  set_BBoxSize(transformPoint3f(this->m_normalizedMatrix, get_BBoxSize()));
}


void vesMapper::render(const vesRenderState &renderState)
{
  if (!this->m_initialized) {
    this->setupDrawObjects(renderState);
  }

  glBindBuffer(GL_ARRAY_BUFFER, this->m_buffer[0]);

  renderState.m_material->bindVertexData(renderState);


  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_buffer[1]);
  glDrawElements(GL_TRIANGLES, this->m_data->GetTriangles().size() * 3, GL_UNSIGNED_SHORT, (void*)0);

  renderState.m_material->unbindVertexData(renderState);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void vesMapper::setupDrawObjects(const vesRenderState &renderState)
{
  const int numberOfFloats = 6;
  size_t sizeOfPositions = (this->m_data->GetPoints().size() * numberOfFloats * sizeof(float));

  // \todo: Put a GL log.
  glGenBuffers(1, &this->m_buffer[0]);
  glBindBuffer(GL_ARRAY_BUFFER, this->m_buffer[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeOfPositions, &this->m_data->GetPoints()[0], GL_STATIC_DRAW);

  glGenBuffers(1, &this->m_buffer[1]);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->m_buffer[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->m_data->GetTriangles().size() *sizeof(unsigned short) * 3,
      &this->m_data->GetTriangles()[0], GL_STATIC_DRAW);

  this->m_initialized = true;
}
