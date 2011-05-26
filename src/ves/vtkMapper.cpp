//
//  vtkMapper.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkMapper.h"


vtkMapper::vtkMapper(vtkFileReader* reader):mFileReader(reader), mData(NULL), m_initialized(false)
{
  this->mIsNew = true;
}
vtkMapper::~vtkMapper()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    glDeleteBuffers(2, mMapperVBO);
  }
//  if(mFileReader)
//  {
//    delete mFileReader;
//  }
  if(mData)
  {
    delete mData;
  }
}

void vtkMapper::Reload(vtkFileReader* reader)
{
  this->mFileReader = reader;
  this->mIsNew = true;
  if(m_initialized) 
  {
    glDeleteBuffers(2, mMapperVBO);
    m_initialized = false;
  }
}

bool vtkMapper::Read()
{
  if(mIsNew) 
  {
    mData = mFileReader->Read();
    mData->ComputeNormals();
    this->mIsNew = false;
  
    if (mFileReader->HasError()) {
      return false;
    }
    glGenBuffers(2, mMapperVBO);
    glBindBuffer(GL_ARRAY_BUFFER, mMapperVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 
                 mData->GetPoints().size() * 6 * sizeof(float),
                 &mData->GetPoints()[0], 
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMapperVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 mData->GetTriangles().size() * 3 * sizeof(unsigned short),
                 &mData->GetTriangles()[0], 
                 GL_STATIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    m_initialized = true;
    return true;
  }
  else
  {
    return false;
  }
}

void vtkMapper::Print(vtkShaderProgram *program)
{
  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  glVertexAttribPointer(program->GetAttribute("a_vertex"), 
                        3, 
                        GL_FLOAT, 
                        0, 
                        6 * sizeof(float), 
                        &mData->GetPoints()[0]);
  glVertexAttribPointer(program->GetAttribute("a_normal"), 
                        3, 
                        GL_FLOAT, 
                        0, 
                        6 * sizeof(float), 
                        mData->GetPoints()[0].normal.mData);
  
  // Draw
  glDrawElements(GL_TRIANGLES, 
                 mData->GetTriangles().size() * 3, 
                 GL_UNSIGNED_SHORT, 
                 &mData->GetTriangles()[0]);
}

vtkPoint3f vtkMapper::GetMin()
{
  return mData->GetMin();
}

vtkPoint3f vtkMapper::GetMax()
{
  return mData->GetMax();
}

vtkTriangleData* vtkMapper::GetTriangleData()
{
  return mData;
}
