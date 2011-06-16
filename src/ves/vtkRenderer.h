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

#ifndef __vtkRenderer_h
#define __vtkRenderer_h

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>
#include <string>
#include "vtkGMTL.h"
#include "vtkCamera.h"
#include "vtkShaderProgram.h"
#include "vtkActor.h"
#include "vtkSceneRender.h"

class vtkRenderer
{
public:
  vtkRenderer(vtkCamera *camera);
  ~vtkRenderer();
  void Render();
  void SetProgram(vtkShaderProgram* program);
  void SetActor(vtkActor* actor);  
  void Read();
  void resetView();
 	void resize(int widht,int height, float scale);
  
protected:
  void CopyCamera2Model();
  
private:
  vtkCamera* mCamera;
  vtkShaderProgram* mProgram;
  vtkActor *mActor;
  
  // Runtime
  vtkMatrix4x4f _model;
  vtkMatrix4x4f _view;
  vtkMatrix4x4f _proj;
  float _width,_height;

  vtkMatrix4x4f m_mv_matrix;
  vtkMatrix3x3f m_normal_matrix;
  
  bool once;
};

#endif
