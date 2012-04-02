/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.
  Copyright 2012 Willow Garage, Inc.

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

#include "vesActor.h"
#include "vesBackground.h"
#include "vesCamera.h"
#include "vesCullVisitor.h"
#include "vesGroupNode.h"
#include "vesRenderer.h"
#include "vesRenderStage.h"
#include "vesShaderProgram.h"
#include "vesVisitor.h"

// C/C++ includes
#include <cassert>
#include <iostream>
#include <string>

vesRenderer::vesRenderer():
  m_width(100),
  m_height(100),
  m_camera(new vesCamera()),
  m_sceneRoot(new vesGroupNode()),
  m_renderStage(new vesRenderStage()),
  m_background (new vesBackground())
{
  this->m_aspect[0] = this->m_aspect[1] = 1.0;

  this->m_camera->addChild(this->m_sceneRoot);

  this->setupBackground();
}


vesRenderer::~vesRenderer()
{
}


void vesRenderer::render()
{
  // By default enable depth test.
  glEnable(GL_DEPTH_TEST);

  if (this->m_sceneRoot) {

    // Update traversal.
    this->updateTraverseScene();

    // Cull traversal.
    this->cullTraverseScene();

    vesRenderState renderState;

    // Clear all the previous render targets.
    this->m_camera->clearRenderTargets(renderState);

    // For now, lets not push camera to the stage, just call
    // render on render target of the current camera.
    this->m_camera->renderTarget()->render(renderState);

    this->m_renderStage->render(renderState, 0);

    // \note: For now clear the stage.
    // \todo: Add an optimization where we could save whole or
    // part of the the stage.
    this->m_renderStage->clearAll();
  }
}


void vesRenderer::resize(int width, int height, float scale)
{
  vesNotUsed(scale);

  this->m_width  = (width  > 0) ? width  : 1;
  this->m_height = (height > 0) ? height : 1;

  this->m_camera->viewport()->setViewport(0, 0, this->m_width, this->m_height);

  this->updateBackgroundViewport();

  this->m_aspect[0] = this->m_camera->viewport()->inverseAspect();
  this->m_aspect[1] = this->m_camera->viewport()->aspect();
}


vesVector3f vesRenderer::computeWorldToDisplay(vesVector3f world)
{
  // WorldToView
  vesMatrix4x4f proj_mat = this->m_camera->computeProjectionTransform(this->m_aspect[1],
                                                                    0, 1);
  vesMatrix4x4f view_mat = this->m_camera->computeViewTransform();
  vesMatrix4x4f t(proj_mat * view_mat);
  vesVector4f world4(world[0], world[1], world[2], 1);
  vesVector4f view = t * world4;
  view[0] /= view[3];
  view[1] /= view[3];
  view[2] /= view[3];

  // ViewToDisplay
  vesVector3f display;
  display[0] = (view[0] + 1.0f) * this->m_width / 2.0f;
  display[1] = (view[1] + 1.0f) * this->m_height / 2.0f;
  display[2] = view[2];
  return display;
}


vesVector3f vesRenderer::computeDisplayToWorld(vesVector3f display)
{
  // DisplayToView
  vesVector4f view;
  view[0] = 2.0f * display[0] / this->m_width - 1.0f;
  view[1] = 2.0f * display[1] / this->m_height - 1.0f;
  view[2] = display[2];
  view[3] = 1;

  // ViewToWorld
  vesMatrix4x4f proj_mat = this->m_camera->computeProjectionTransform(this->m_aspect[1],
                                                                    0, 1);
  vesMatrix4x4f view_mat = this->m_camera->computeViewTransform();
  vesMatrix4x4f mat = proj_mat*view_mat;
  vesMatrix4x4f inv = mat.inverse();
  vesVector4f world4 = inv * view;

  vesVector3f world(world4[0]/world4[3], world4[1]/world4[3],
                    world4[2]/world4[3]);
  return world;
}


void vesRenderer::resetCamera()
{
  if (!this->m_sceneRoot) {
    return;
  }

  // If bounds of scene are dirty make a update traversal
  // to make bounds upto date.
  if (this->m_sceneRoot->boundsDirty()) {
    this->updateTraverseScene();
  }

  vesVector3f center = this->m_sceneRoot->boundsCenter();

  double distance;
  vesVector3f vn, vup;

  if ( this->m_camera != NULL ) {
    vn = this->m_camera->viewPlaneNormal();
  }
  else
  {
    std::cerr<< "Trying to reset non-existant camera" << std::endl;
    return;
  }

  double radius = this->m_sceneRoot->boundsRadius();

  radius = (radius==0)? (.5) : (radius);

  double angle = deg2Rad(this->m_camera->viewAngle());
  double parallelScale = radius;

  // Horizontal window, deal with vertical angle|scale
  if (this->m_aspect[0]>=1.0) {
    if (this->m_camera->useHorizontalViewAngle()) {
      angle=2.0*atan(tan(angle*0.5)/this->m_aspect[0]);
    }
  }
  // Vertical window, deal with horizontal angle|scale
  else {
    if (!this->m_camera->useHorizontalViewAngle() ) {
      angle=2.0*atan(tan(angle*0.5)*this->m_aspect[0]);
    }
    parallelScale=parallelScale/this->m_aspect[0];
  }

  distance =radius/sin(angle*0.5);

  // Check view-up vector against view plane normal
  vup = this->m_camera->viewUp();
  if ( fabs(vup.dot(vn)) > 0.999 ) {
    // vtkWarningMacro(<<"Resetting view-up since view plane normal is parallel");
    vesVector3f temp;
    temp[0] = -vup[2];
    temp[1] = vup[0];
    temp[2] = vup[1];
    this->m_camera->setViewUp(temp);
  }

  // Update the camera
  this->m_camera->setFocalPoint(center);
  vesVector3f temp = vn;
  temp*= distance;
  temp+= center;
  this->m_camera->setPosition(temp);

  this->resetCameraClippingRange();

  // Setup default parallel scale
  this->m_camera->setParallelScale(parallelScale);
}


void vesRenderer::resetCameraClippingRange()
{
  if (!this->m_sceneRoot) {
    return;
  }

  // If bounds of scene are dirty make a update traversal
  // to make bounds upto date.
  if (this->m_sceneRoot->boundsDirty()) {
    this->updateTraverseScene();
  }

  float bounds[6];

  vesVector3f minimumBounds = this->m_sceneRoot->boundsMinimum();
  vesVector3f maximumBounds = this->m_sceneRoot->boundsMaximum();

  bounds[0] = minimumBounds[0];
  bounds[1] = maximumBounds[0];
  bounds[2] = minimumBounds[1];
  bounds[3] = maximumBounds[1];
  bounds[4] = minimumBounds[2];
  bounds[5] = maximumBounds[2];

  this->resetCameraClippingRange(bounds);
}


void vesRenderer::resetCameraClippingRange(float bounds[6])
{
  vesVector3f  vn, position;
  float  a, b, c, d;
  double range[2], dist;
  int    i, j, k;

  // Find the plane equation for the camera view plane
  vn = this->m_camera->viewPlaneNormal();
  position = this->m_camera->position();

  a = -vn[0];
  b = -vn[1];
  c = -vn[2];
  d = -(a*position[0] + b*position[1] + c*position[2]);

  // Set the max near clipping plane and the min far clipping plane
  range[0] = a*bounds[0] + b*bounds[2] + c*bounds[4] + d;
  range[1] = 1e-18;

  // Find the closest / farthest bounding box vertex
  for ( k = 0; k < 2; k++ )
  {
    for ( j = 0; j < 2; j++ )
    {
      for ( i = 0; i < 2; i++ )
      {
        dist = a*bounds[i] + b*bounds[2+j] + c*bounds[4+k] + d;
        range[0] = (dist<range[0])?(dist):(range[0]);
        range[1] = (dist>range[1])?(dist):(range[1]);
      }
    }
  }

  // Do not let the range behind the camera throw off the calculation.
  if (range[0] < 0.0) {
    range[0] = 0.0;
  }

  // Give ourselves a little breathing room
  range[0] = 0.99*range[0] - (range[1] - range[0])*0.5;
  range[1] = 1.01*range[1] + (range[1] - range[0])*0.5;

  // Make sure near is not bigger than far
  range[0] = (range[0] >= range[1])?(0.01*range[1]):(range[0]);

  float NearClippingPlaneTolerance = 0.01;

  // Make sure the front clipping range is not too far from the far clippnig
  // range, this is to make sure that the zbuffer resolution is effectively
  // used.

  if (range[0] < NearClippingPlaneTolerance*range[1]) {
    range[0] = NearClippingPlaneTolerance*range[1];
  }
  this->m_camera->setClippingRange( range[0],range[1] );
}


void vesRenderer::setBackgroundColor(float r, float g, float b, float a)
{
  if (this->m_background) {
    this->m_background->setColor(vesVector4f(r, g, b, a));
  }
  this->m_camera->setClearColor(vesVector4f(r, g, b, a));
}


vesSharedPtr<vesBackground> vesRenderer::background()
{
  return this->m_background;
}


const vesSharedPtr<vesBackground> vesRenderer::background() const
{
  return this->m_background;
}


void vesRenderer::addActor(vesSharedPtr<vesActor> actor)
{
  if (actor) {
    this->m_sceneRoot->addChild(actor);
  }
}


void vesRenderer::removeActor(vesSharedPtr<vesActor> actor)
{
  if (actor) {
    this->m_sceneRoot->removeChild(actor);
  }
}


void vesRenderer::updateTraverseScene()
{
  // Update traversal.
  vesVisitor updateVisitor(vesVisitor::UpdateVisitor,
                           vesVisitor::TraverseAllChildren);

  this->m_camera->accept(updateVisitor);
}


void vesRenderer::cullTraverseScene()
{
  vesCullVisitor cullVisitor;

  vesMatrix4x4f projection2DMatrix = vesOrtho(0, this->width(), 0, this->height(), -1, 1);
  cullVisitor.setProjection2DMatrix(projection2DMatrix);

  cullVisitor.setRenderStage(this->m_renderStage);

  this->m_camera->accept(cullVisitor);
}


void vesRenderer::setupBackground()
{
  this->updateBackgroundViewport();
  this->m_camera->addChild(this->m_background);
  this->m_camera->setClearMask(vesStateAttributeBits::DepthBufferBit);
}


void vesRenderer::updateBackgroundViewport()
{
  assert(this->m_camera);
  assert(this->m_background);

  this->m_background->viewport()->setViewport(
    this->m_camera->viewport()->x(), this->m_camera->viewport()->y(),
    this->m_camera->viewport()->width(), this->m_camera->viewport()->height());
}
