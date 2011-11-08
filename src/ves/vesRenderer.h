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

// VES includes
#include "vesGL.h"
#include "vesGMTL.h"
#include "vesSetGet.h"

// C++ includes
#include <string>

// Forward declarations
class vesActor;
class vesBackground;
class vesCamera;
class vesGroupNode;
class vesRenderStage;
class vesTexture;

class vesRenderer
{
public:
  vesTypeMacro(vesRenderer);

  vesRenderer();
  virtual ~vesRenderer();

  virtual void render();

  virtual void resetCamera();
  virtual void resetCameraClippingRange();
  virtual void resize(int width,int height, float scale);

  virtual void setBackgroundColor(float r, float g, float b, float a=1.0f);
  vesSharedPtr<vesBackground> background();
  const vesSharedPtr<vesBackground> background() const;

  virtual void addActor   (vesSharedPtr<vesActor> actor);
  virtual void removeActor(vesSharedPtr<vesActor> actor);

  const  vesSharedPtr<vesGroupNode> sceneRoot() const { return this->m_sceneRoot; }

  inline vesSharedPtr<vesCamera> camera(){ return this->m_camera; }

  inline int width()   { return this->m_width;  }
  inline int height()  { return this->m_height; }

  vesVector3f computeWorldToDisplay(vesVector3f world);
  vesVector3f computeDisplayToWorld(vesVector3f display);

protected:

  virtual void updateTraverseScene();
  virtual void cullTraverseScene();
  virtual void setupBackground();
  virtual void updateBackgroundViewport();

  void resetCameraClippingRange(float bounds[6]);

private:
  double m_aspect[2];
  int m_width;
  int m_height;

  vesSharedPtr<vesCamera> m_camera;
  vesSharedPtr<vesGroupNode> m_sceneRoot;

  vesSharedPtr<vesRenderStage> m_renderStage;
  vesSharedPtr<vesBackground> m_background;
};

#endif
