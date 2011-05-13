//
//  vtkMapper.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkMapper.h"


vtkMapper::vtkMapper(vtkFileReader* reader):mFileReader(reader), m_initialized(true)
{
  
}
vtkMapper::~vtkMapper()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    glDeleteBuffers(2, mMapperVBO);
  }
  if(mFileReader)
  {
    delete mFileReader;
  }
}

void vtkMapper::Read()
{
  mFileReader->Read();
  glGenBuffers(2, mMapperVBO);
  glBindBuffer(GL_ARRAY_BUFFER, mMapperVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, 
               mFileReader->m_numPoints * 6 * sizeof(float),
               mFileReader->m_points, 
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mMapperVBO[1]);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
               mFileReader->m_triangles.size() * 3 * sizeof(unsigned short),
               &mFileReader->m_triangles[0], 
               GL_STATIC_DRAW);
  
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void vtkMapper::Print(vtkShaderProgram *program)
{
  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
  glVertexAttribPointer(program->GetAttribute("a_vertex"), 
                        3, 
                        GL_FLOAT, 
                        0, 
                        6 * sizeof(float), 
                        mFileReader->m_points);
  glVertexAttribPointer(program->GetAttribute("a_normal"), 
                        3, 
                        GL_FLOAT, 
                        0, 
                        6 * sizeof(float), 
                        mFileReader->m_points[0].normal.mData);
  
  // Draw
  glDrawElements(GL_TRIANGLES, 
                 mFileReader->m_triangles.size() * 3, 
                 GL_UNSIGNED_SHORT, 
                 &mFileReader->m_triangles[0]);
}

vtkPoint3f vtkMapper::GetMin()
{
  return mFileReader->min;
}

vtkPoint3f vtkMapper::GetMax()
{
  return mFileReader->max;
}

