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
#include "vtkMatrix.h"
#include <string>
#include "vtkMultitouchNavMatrix.h"

class vtkFileReader;
class vtkShaderProgram;
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

  void readFiles(int file = 0);

  void resetView();
  
  void release();
	void resize(int widht,int height, float scale);

  void setFilePath(const std::string &path)
  {
    filePath = path;
    newFile = 1;
  }

  void Cross(const float x[3], const float y[3], float z[3])
  {
    float Zx = x[1] * y[2] - x[2] * y[1];
    float Zy = x[2] * y[0] - x[0] * y[2];
    float Zz = x[0] * y[1] - x[1] * y[0];
    z[0] = Zx; z[1] = Zy; z[2] = Zz;
  }

  float Norm(const float x[3])
  {
    return static_cast<float> (sqrt( x[0] * x[0] + x[1] * x[1] + x[2] * x[2] ) );
  }

  float Normalize(float x[3])
  {
    float den;
    if ( ( den = Norm( x ) ) != 0.0 )
      {
        for (int i=0; i < 3; i++)
          {
            x[i] /= den;
          }
      }
    return den;
  }

private:
  vtkMultitouchNavMatrix _nav;
  vtkShaderProgram* Program;
  // Runtime
  vtkMatrix4x4f _model;
  vtkMatrix4x4f _view;
  vtkMatrix4x4f _proj;
  float _width,_height;

  vtkMatrix4x4f m_mv_matrix;
  vtkMatrix3x3f m_normal_matrix;
  double m_rotation;

	std::string filePath;
	bool newFile;
	vtkFileReader *m_newFile;
	GLuint mNewFileVBO[2];
  vtkFileReader *m_sphere;
  GLuint m_sphere_vbo[2];

  int m_view;
  bool m_initialized;
  int m_renderNumber;
};

#endif
