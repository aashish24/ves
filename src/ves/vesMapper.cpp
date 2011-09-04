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

vesMapper::vesMapper(): Data(NULL), m_initialized(false)
{
  this->mIsNew = true;
  this->Red = 0.8;
  this->Green = 0.8;
  this->Blue = 0.8;
  this->Alpha = 1.0;
  this->DrawPoints = false;
}
vesMapper::~vesMapper()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    //glDeleteBuffers(2, mMapperVBO);
  }
}

void vesMapper::SetTriangleData(vesTriangleData* data)
{
  this->Data = data;
}

void vesMapper::SetColor(float r, float g, float b, float a)
{
  this->Red = r;
  this->Green = g;
  this->Blue = b;
  this->Alpha =a;
}

vesMatrix4x4f vesMapper::Eval()
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
                        &this->Data->GetPoints()[0]);
  glVertexAttribPointer(program->GetAttribute("a_normal"),
                        3,
                        GL_FLOAT,
                        0,
                        6 * sizeof(float),
                        this->Data->GetPoints()[0].normal.mData);

  // draw triangles
  glDrawElements(GL_TRIANGLES,
                 this->Data->GetTriangles().size() * 3,
                 GL_UNSIGNED_SHORT,
                 &this->Data->GetTriangles()[0]);

  // draw lines
  glDrawElements(GL_LINES,
                 this->Data->GetLines().size() * 2,
                 GL_UNSIGNED_SHORT,
                 &this->Data->GetLines()[0]);
}


vesTriangleData* vesMapper::GetTriangleData()
{
  return this->Data;
}

void vesMapper::computeBounds()
{
  vesVector3f min = this->Data->GetMin();
  vesVector3f max = this->Data->GetMax();
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

void vesMapper::Normalize()
{
  float r = GetBBoxRadius();
  this->NormalizedMatrix =
      makeScaleMatrix4x4(1/r,1/r,1/r)*
      makeTranslationMatrix4x4(-get_BBoxCenter());
  set_BBoxCenter(transformPoint3f(this->NormalizedMatrix, get_BBoxCenter()));
  set_BBoxSize(transformPoint3f(this->NormalizedMatrix, get_BBoxSize()));
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
  render->Mapper(this);
}

vesTriangleData* vesMapper::GetData()
{
  return this->Data;
}

