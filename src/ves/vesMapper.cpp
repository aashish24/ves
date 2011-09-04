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

#include "Painter.h"
#include "vesShaderProgram.h"
#include "vesTriangleData.h"

vesMapper::vesMapper(): m_data(NULL), m_initialized(false)
{
  this->m_isNew = true;
  this->m_red = 0.8;
  this->m_green = 0.8;
  this->m_blue = 0.8;
  this->m_alpha = 1.0;
  this->m_drawPoints = false;
}
vesMapper::~vesMapper()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    //glDeleteBuffers(2, mMapperVBO);
  }
}

void vesMapper::setTriangleData(vesTriangleData* data)
{
  this->m_data = data;
}

void vesMapper::setColor(float r, float g, float b, float a)
{
  this->m_red = r;
  this->m_green = g;
  this->m_blue = b;
  this->m_alpha =a;
}

vesMatrix4x4f vesMapper::eval()
{
  vesMatrix4x4f temp;
  return temp;
  //  vesMatrix4x4f temp= makeTransposeMatrix4x4(makeTransposeMatrix4x4(this->NormalizedMatrix));
  //  return temp;
}

bool vesMapper::read()
{
  return true;
}

void vesMapper::render(vesShaderProgram *program)
{
  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  glVertexAttribPointer(program->GetAttribute("a_vertex"),
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        &this->m_data->GetPoints()[0]);
  glVertexAttribPointer(program->GetAttribute("a_normal"),
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        this->m_data->GetPoints()[0].normal.mData);

  // draw triangles
  glDrawElements(GL_TRIANGLES,
                 this->m_data->GetTriangles().size() * 3,
                 GL_UNSIGNED_SHORT,
                 &this->m_data->GetTriangles()[0]);

  // draw lines
  glDrawElements(GL_LINES,
                 this->m_data->GetLines().size() * 2,
                 GL_UNSIGNED_SHORT,
                 &this->m_data->GetLines()[0]);
}


vesTriangleData* vesMapper::triangleData()
{
  return this->m_data;
}

void vesMapper::computeBounds()
{
  vesVector3f min = this->m_data->GetMin();
  vesVector3f max = this->m_data->GetMax();
  set_BBoxSize(min,max);
  set_BBoxCenter(min, max);
  /*
  std::cout<< "BBoxSize = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxSize()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;

  std::cout<< "BBoxCenter = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxCenter()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;
  */
}

void vesMapper::normalize()
{
  float r = GetBBoxRadius();
  this->m_normalizedMatrix =
      makeScaleMatrix4x4(1/r,1/r,1/r)*
      makeTranslationMatrix4x4(-get_BBoxCenter());
  set_BBoxCenter(transformPoint3f(this->m_normalizedMatrix, get_BBoxCenter()));
  set_BBoxSize(transformPoint3f(this->m_normalizedMatrix, get_BBoxSize()));
  /*
  std::cout<< "BBoxSize = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxSize()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;

  std::cout<< "BBoxCenter = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxCenter()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;
  */
}

void vesMapper::render(Painter* render)
{
}

vesTriangleData* vesMapper::data()
{
  return this->m_data;
}

