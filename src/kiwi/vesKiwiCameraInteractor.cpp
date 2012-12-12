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

#include "vesKiwiCameraInteractor.h"
#include "vesCamera.h"
#include "vesRenderer.h"

#include <cstdio>

//----------------------------------------------------------------------------
vesKiwiCameraInteractor::vesKiwiCameraInteractor()
{
}

//----------------------------------------------------------------------------
vesKiwiCameraInteractor::~vesKiwiCameraInteractor()
{
}

//----------------------------------------------------------------------------
void vesKiwiCameraInteractor::setRenderer(vesRenderer::Ptr renderer)
{
  mRenderer = renderer;
}

//----------------------------------------------------------------------------
vesRenderer::Ptr vesKiwiCameraInteractor::renderer() const
{
  return mRenderer;
}

//----------------------------------------------------------------------------
void vesKiwiCameraInteractor::dolly(double scale)
{
  vesCamera::Ptr camera = mRenderer->camera();
  if (camera->parallelProjection()) {
    camera->setParallelScale((1.0/scale)*camera->parallelScale());
  }
  else {
    mRenderer->camera()->dolly(scale);
  }
}

//----------------------------------------------------------------------------
void vesKiwiCameraInteractor::roll(double rotation)
{
  vesCamera::Ptr camera = mRenderer->camera();
  camera->roll(rotation * 180.0 / M_PI);
  camera->orthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiCameraInteractor::rotate(vesVector2d screenTranslation)
{
  //
  // Rotate camera
  // Based on vtkInteractionStyleTrackballCamera::Rotate().
  //
  double delta_elevation = -20.0 / mRenderer->height();
  double delta_azimuth   = -20.0 / mRenderer->width();

  double motionFactor = 10.0;

  double rxf = screenTranslation[0] * delta_azimuth * motionFactor;
  double ryf = screenTranslation[1] * delta_elevation * motionFactor;

  vesCamera::Ptr camera = mRenderer->camera();
  camera->azimuth(rxf);
  camera->elevation(ryf);
  camera->orthogonalizeViewUp();
}

//----------------------------------------------------------------------------
void vesKiwiCameraInteractor::pan(vesVector2d p0, vesVector2d p1)
{
  // calculate the focal depth so we'll know how far to move
  vesCamera::Ptr camera = mRenderer->camera();
  vesVector3f viewFocus = camera->focalPoint();
  vesVector3f viewPoint = camera->position();
  vesVector3f viewFocusDisplay = mRenderer->computeWorldToDisplay(viewFocus);
  float focalDepth = viewFocusDisplay[2];

  // map change into world coordinates
  vesVector3f oldPickPoint = mRenderer->computeDisplayToWorld(vesVector3f(p0[0], p0[1], focalDepth));
  vesVector3f newPickPoint = mRenderer->computeDisplayToWorld(vesVector3f(p1[0], p1[1], focalDepth));
  vesVector3f motionVector = oldPickPoint - newPickPoint;

  vesVector3f newViewFocus = viewFocus + motionVector;
  vesVector3f newViewPoint = viewPoint + motionVector;

  camera->setFocalPoint(newViewFocus);
  camera->setPosition(newViewPoint);
}
