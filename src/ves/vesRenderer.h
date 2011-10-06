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

#ifndef VESRENDERER_H
#define VESRENDERER_H

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#endif

// VES includes
#include "vesGMTL.h"
#include "vesMultitouchCamera.h"
#include "vesShaderProgram.h"
#include "vesActor.h"
#include "vesSceneRender.h"
#include "vesRenderStage.h"
#include "vesCamera.h"

// C++ includes
#include <string>

// Forward declarations
class vesRenderStage;
class vesTexture;

class vesRenderer
{
public:
           vesRenderer();
  virtual ~vesRenderer();

  virtual void render();

  virtual void resetCamera();
  virtual void resetCameraClippingRange();
  virtual void resize(int width,int height, float scale);

  virtual void setBackgroundColor(float r, float g, float b, float a=1.0f);
  virtual void setBackground(vesTexture *background);

  void setSceneRoot(vesActor *root);

  inline vesCamera* camera(){ return this->m_camera; }

  inline int width()   { return this->m_width;  }
  inline int height()  { return this->m_height; }

  vesVector3f computeWorldToDisplay(vesVector3f world);
  vesVector3f computeDisplayToWorld(vesVector3f display);


protected:

  virtual void updateTraverseScene();
  virtual void cullTraverseScene();

  void resetCameraClippingRange(float bounds[6]);


private:
  double     m_aspect[2];
  int        m_width;
  int        m_height;
  float      m_backgroundColor[4];

  vesCamera *m_camera;
  vesActor  *m_sceneRoot;

  vesRenderStage *m_renderStage;
};

#endif
