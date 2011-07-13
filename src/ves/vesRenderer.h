/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vesRenderer.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef __vesRenderer_h
#define __vesRenderer_h

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#import <QuartzCore/QuartzCore.h>
#include <string>
#include "vesGMTL.h"
#include "vesMultitouchCamera.h"
#include "vesShaderProgram.h"
#include "vesActor.h"
#include "vesSceneRender.h"
#include "vtkPainter.h"

class vesActorCollection;

class vesRenderer
{
public:
  vesRenderer(vesMultitouchCamera *camera);
  ~vesRenderer();
  void Render();
  void AddActor(vesActor* actor);
  void RemoveActor(vesActor* actor);
  void Read();
  void resetView();
  void resize(int widht,int height, float scale);

protected:
  void CopyCamera2Model();

private:
  vesMultitouchCamera* mCamera;
  vesActorCollection *Actor;
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
