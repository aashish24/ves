/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

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
  void Resize(int width,int height, float scale);
  int GetWidth() { return Width; }
  int GetHeight() { return Height; }
  void ResetCamera();
  void ResetCameraClippingRange();
  void SetBackground(vesTexture* background);
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
