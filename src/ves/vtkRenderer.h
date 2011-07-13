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
#include "vesGMTL.h"
#include "vtkMultitouchCamera.h"
#include "vtkShaderProgram.h"
#include "vtkActor.h"
#include "vtkSceneRender.h"
#include "vtkPainter.h"

class vtkActorCollection;

class vtkRenderer
{
public:
  vtkRenderer(vtkMultitouchCamera *camera);
  ~vtkRenderer();
  void Render();
  void AddActor(vtkActor* actor);
  void RemoveActor(vtkActor* actor);
  void Read();
  void resetView();
  void resize(int widht,int height, float scale);

protected:
  void CopyCamera2Model();

private:
  vtkMultitouchCamera* mCamera;
  vtkActorCollection *Actor;
  vtkPainter *Painter;
  // Runtime
  vesMatrix4x4f _model;
  vesMatrix4x4f _view;
  vesMatrix4x4f _proj;
  float _width,_height;

  vesMatrix4x4f m_mv_matrix;
  vesMatrix3x3f m_normal_matrix;

  bool once;
};

#endif
