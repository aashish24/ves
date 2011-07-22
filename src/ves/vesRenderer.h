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

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

#include <string>
#include "vesGMTL.h"
#include "vesMultitouchCamera.h"
#include "vesShaderProgram.h"
#include "vesActor.h"
#include "vesSceneRender.h"
#include "Painter.h"
#include "vesCamera.h"

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
  vesCamera* GetActiveCamera();
#if VTK
  void ResetCamera();
protected:
  void ResetCameraClippingRange(float bounds[6]);
#endif

protected:
  void CopyCamera2Model();

private:
  vesMultitouchCamera* mCamera;
  vesActorCollection *Actor;
  vesCamera *ActiveCamera;
  Painter *_Painter;
  // Runtime
  vesMatrix4x4f _model;
  vesMatrix4x4f _view;
  vesMatrix4x4f _proj;
  float _width,_height;

  vesMatrix4x4f m_mv_matrix;
  vesMatrix3x3f m_normal_matxrix;

  float Aspect[2];
  bool once;
};

#endif
