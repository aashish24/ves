/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkRenderer.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef RENDERER_H
#define RENDERER_H

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>
#include "gmtl/Math.h"
//include "vtkMatrix.h"
#include <string>
#include "vtkCamera.h"
#include "vtkGMTL.h"
#include "vtkFileReader.h"
#include "vtkShaderProgram.h"

class vtkActor
{
public:
  vtkActor(vtkFileReader* reader): 
              mFileReader(reader),
              m_initialized(true)
  {
    mMapperVBO[0] = mMapperVBO[1] = 0;
  }
  void Release()
  {
    // Release our VBOs before the context is destroyed
    if (m_initialized) {
      glDeleteBuffers(2, mMapperVBO);
    }
  }
  void read()
  {
    mFileReader->read();
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
  
  void Print(vtkShaderProgram *program)
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

  vtkVector3f center;
  float radius;

protected:
  void ComputeCenterAndRadius(vtkVector3f min, vtkVector3f max)
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

  vtkMatrix4x4f mMatrix;
  bool once;
  vtkFileReader *mFileReader;
  bool m_initialized;
  GLuint mMapperVBO[2];
};

class vtkRenderer
{
public:
  vtkRenderer();

  void Render();

  void Render(float xrot, float yrot, float scale, float xtrans, float ytrans);

  void SetProgram(vtkShaderProgram* program)
  {
    this->Program = program;
  }
  
  void SetActor(vtkActor* actor)
  {
    this->mActor = actor;
  }
  
  void read();

  void resetView();
  void release();
	void resize(int widht,int height, float scale);

  

private:
  vtkCamera mCamera;
  vtkShaderProgram* Program;
  // Runtime
  vtkMatrix4x4f _model;
  vtkMatrix4x4f _view;
  vtkMatrix4x4f _proj;
  float _width,_height;

  vtkMatrix4x4f m_mv_matrix;
  vtkMatrix3x3f m_normal_matrix;
  double m_rotation;
  vtkActor *mActor;
  bool once;
};

#endif
