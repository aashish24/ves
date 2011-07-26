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
  vesRenderer();
  ~vesRenderer();
  void Render();
  void AddActor(vesActor* actor);
  void RemoveActor(vesActor* actor);
  vesCamera* GetCamera() { return Camera; }
  void Resize(int widht,int height, float scale);
  void ResetCamera();
  vesVector3f ComputeWorldToDisplay(vesVector3f world);
  vesVector3f ComputeDisplayToWorld(vesVector3f display);

protected:
  void ResetCameraClippingRange(float bounds[6]);

private:
  vesActorCollection *Actor;
  Painter* Paint;
  vesCamera* Camera;
  double Aspect[2];
  int Width;
  int Height;
};

#endif
