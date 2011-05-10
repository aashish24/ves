//
//  vtkActor.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkActor.h"

vtkActor::vtkActor(vtkFileReader* reader): mFileReader(reader), m_initialized(true)
{
  mMapperVBO[0] = mMapperVBO[1] = 0;
}
void vtkActor::Release()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    glDeleteBuffers(2, mMapperVBO);
  }
}
void vtkActor::Read()
{
  mFileReader->Read();
  ComputeCenterAndRadius(mFileReader->min,mFileReader->max);
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

void vtkActor::Print(vtkShaderProgram *program)
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

void vtkActor::ComputeCenterAndRadius(vtkVector3f min, vtkVector3f max)
{
  center[0] = (min[0]+max[0])/2;
  center[1] = (min[1]+max[1])/2;
  center[2] = (min[2]+max[2])/2;
  float x = max[0]-min[0];
  float y = max[1]-min[1];
  float z = max[2]-min[2];
  radius = sqrt(x*x+y*y+z*z);
  
  std::cout << "min    = ( "<< min[0] << " " << min[1] << " " << min[2] << ")" << std::endl;
  std::cout << "max    = ( "<< max[0] << " " << max[1] << " " << max[2] << ")" << std::endl;
  std::cout << "Center = ( "<< center[0] << " " << center[1] << " " << center[2] << ")" << std::endl;
  std::cout << "Radius = " << radius <<std::endl;
}

