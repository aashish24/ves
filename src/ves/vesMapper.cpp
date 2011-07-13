//
//  vesMapper.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vesMapper.h"
#include "Painter.h"

vesMapper::vesMapper(vesFileReader* reader):mFileReader(reader), Data(NULL), m_initialized(false)
{
  this->mIsNew = true;
}
vesMapper::~vesMapper()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
      //glDeleteBuffers(2, mMapperVBO);
  }
//  if(mFileReader)
//  {
//    delete mFileReader;
//  }
  if(this->Data)
  {
    delete this->Data;
  }
}

void vesMapper::Reload(vesFileReader* reader)
{
  this->mFileReader = reader;
  this->mIsNew = true;
  if(m_initialized)
  {
    //glDeleteBuffers(2, mMapperVBO);
    m_initialized = false;
  }
}

vesMatrix4x4f vesMapper::Eval()
{
  vesMatrix4x4f temp;
  return temp;
//  vesMatrix4x4f temp= makeTransposeMatrix4x4(makeTransposeMatrix4x4(this->NormalizedMatrix));
//  return temp;
}

bool vesMapper::Read()
{
  std::cout << "Read: Mapper" <<std::endl;
  if(mIsNew)
  {
    this->Data = mFileReader->Read();
    this->Data->ComputeNormals();
    this->mIsNew = false;

    if (mFileReader->HasError()) {
      return false;
    }
//    glGenBuffers(2, mMapperVBO);
//    glBindBuffer(GL_ARRAY_BUFFER, mMapperVBO[0]);
//    glBufferData(GL_ARRAY_BUFFER,
//                 mData->GetPoints().size() * 6 * sizeof(float),
//                 &mData->GetPoints()[0],
//                 GL_STATIC_DRAW);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMapperVBO[1]);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
//                 mData->GetTriangles().size() * 3 * sizeof(unsigned short),
//                 &mData->GetTriangles()[0],
//                 GL_STATIC_DRAW);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_initialized = true;
   //Normalize();
    return true;
  }
  else
  {
    return false;
  }
}

void vesMapper::Render(vesShaderProgram *program)
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

void vesMapper::ComputeBounds()
{
  vesVector3f min = this->Data->GetMin();
  vesVector3f max = this->Data->GetMax();
  SetBBoxSize(min,max);
  SetBBoxCenter(min, max);
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
}

void vesMapper::Normalize()
{
  float r = GetBBoxRadius();
  this->NormalizedMatrix =
  makeScaleMatrix4x4(1/r,1/r,1/r)*
  makeTranslationMatrix4x4(-GetBBoxCenter());
  SetBBoxCenter(transformPoint3f(this->NormalizedMatrix, GetBBoxCenter()));
  SetBBoxSize(transformPoint3f(this->NormalizedMatrix, GetBBoxSize()));
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
}

void vesMapper::Render(Painter* render)
{
  render->Mapper(this);
}

vesTriangleData* vesMapper::GetData()
{
  return this->Data;
}

