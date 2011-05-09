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
#include "vtkCamera.h"

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

private:
  vtkCamera _nav;
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
