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
#include "vtkActor.h"

class vtkRenderer
{
public:
  vtkRenderer();
  ~vtkRenderer();

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
  
  void Read();

  void resetView();
  //void release();
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
