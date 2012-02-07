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

#include "vesCamera.h"

// VES includes
#include "vesStateAttributeBits.h"
#include "vesRenderStage.h"
#include "vesRenderState.h"
#include "vesViewport.h"
#include "vesVisitor.h"

// C/C++ includes
#include <algorithm>
#include <cassert>
#include <iostream>

vesCamera::vesCamera() : vesTransformNode()
{
  this->setReferenceFrame(Absolute);

  this->m_focalPoint[0] = 0.0;
  this->m_focalPoint[1] = 0.0;
  this->m_focalPoint[2] = 0.0;

  this->m_position[0] = 0.0;
  this->m_position[1] = 0.0;
  this->m_position[2] = 1.0;

  this->m_viewUp[0] = 0.0;
  this->m_viewUp[1] = 1.0;
  this->m_viewUp[2] = 0.0;

  this->m_directionOfProjection[0] = 0.0;
  this->m_directionOfProjection[1] = 0.0;
  this->m_directionOfProjection[2] = 0.0;

  this->m_viewAngle = 30.0;
  this->m_useHorizontalViewAngle = 0;

  this->m_clippingRange[0] = 10.0f;
  this->m_clippingRange[1] = 1010.0f;

  this->m_parallelProjection = false;
  this->m_parallelScale = 1.0;

  this->m_windowCenter[0] = 0.0;
  this->m_windowCenter[1] = 0.0;

  this->m_viewport = vesViewport::Ptr(new vesViewport());

  this->m_renderOrder = PostRender;

  this->m_renderOrderPriority = 0;

  this->m_renderTargetStack.push_back(new vesRenderTarget());

  this->m_clearMask = vesStateAttributeBits::ColorBufferBit
    | vesStateAttributeBits::DepthBufferBit;
  this->m_clearColor = vesVector4f(1.0f, 1.0f, 1.0f, 1.0f);
  this->m_clearDepth = 1.0;

  this->computeDistance();
}


vesCamera::~vesCamera()
{
  // Delete only the first (default render target).
  assert(!this->m_renderTargetStack.empty());

  delete this->m_renderTargetStack[0];
}


vesMatrix4x4f vesCamera::computeViewTransform()
{
  return vesLookAt (this->m_position,
                    this->m_focalPoint,
                    this->m_viewUp);
}


vesMatrix4x4f vesCamera::computeProjectionTransform(float aspect,
                                                    float nearz,
                                                    float farz)
{
  vesMatrix4x4f matrix;
  matrix.setIdentity();

  // adjust Z-buffer range
  matrix(2, 2) = (farz - nearz) / (1 - (-1));
  matrix(2, 3) = (nearz*1 - farz*(-1)) / (1 - (-1));

  if (this->m_parallelProjection)
  {
    // set up a rectangular parallelipiped

    double width = this->m_parallelScale * aspect;
    double height = this->m_parallelScale;

    double xmin = ( this->m_windowCenter[0] - 1.0 ) * width;
    double xmax = ( this->m_windowCenter[0] + 1.0 ) * width;
    double ymin = ( this->m_windowCenter[1] - 1.0 ) * height;
    double ymax = ( this->m_windowCenter[1] + 1.0 ) * height;

    vesMatrix4x4f ortho = vesOrtho( xmin, xmax, ymin, ymax,
                                    this->m_clippingRange[0],
                                    this->m_clippingRange[1] );
    return matrix * ortho;
  }
  else
  {
    // set up a perspective frustum
    double tmp = tan( deg2Rad( this->m_viewAngle ) / 2. );
    double width;
    double height;
    if (this->m_useHorizontalViewAngle)
    {
      width = this->m_clippingRange[0] * tmp;
      height = this->m_clippingRange[0] * tmp / aspect;
    }
    else
    {
      width = this->m_clippingRange[0] * tmp * aspect;
      height = this->m_clippingRange[0] * tmp;
    }

    double xmin = ( this->m_windowCenter[0] - 1.0 ) * width;
    double xmax = ( this->m_windowCenter[0] + 1.0 ) * width;
    double ymin = ( this->m_windowCenter[1] - 1.0 ) * height;
    double ymax = ( this->m_windowCenter[1] + 1.0 ) * height;

    vesMatrix4x4f frustum = vesFrustum( xmin, xmax, ymin, ymax,
                                        this->m_clippingRange[0],
                                        this->m_clippingRange[1] );
    return matrix * frustum;
  }
}


// Rotate the camera about the view up vector centered at the focal point.
void vesCamera::azimuth(double angle)
{
  vesVector3f fp = this->m_focalPoint;
  vesVector3f vu = this->m_viewUp;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(deg2Rad(angle), vu[0], vu[1], vu[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesAffine3f t(t1 * t2 * t3);

  vesVector4f oldPosition(this->m_position[0], this->m_position[1], this->m_position[2], 1);
  vesVector4f newPosition = t * oldPosition;
  this->m_position[0] = newPosition[0] / newPosition[3];
  this->m_position[1] = newPosition[1] / newPosition[3];
  this->m_position[2] = newPosition[2] / newPosition[3];
  this->computeDistance();
}


// Rotate the camera about the cross product of the negative of the
// direction of projection and the view up vector centered on the focal point.
void vesCamera::elevation(double angle)
{
  vesMatrix4x4f view = this->computeViewTransform();
  vesVector3f axis;
  axis[0] = -view(0, 0);
  axis[1] = -view(0, 1);
  axis[2] = -view(0, 2);
  vesVector3f fp = this->m_focalPoint;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(-deg2Rad(angle), axis[0], axis[1], axis[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesAffine3f t(t1 * t2 * t3);

  vesVector4f oldPosition(this->m_position[0], this->m_position[1], this->m_position[2], 1);
  vesVector4f newPosition = t * oldPosition;
  this->m_position[0] = newPosition[0] / newPosition[3];
  this->m_position[1] = newPosition[1] / newPosition[3];
  this->m_position[2] = newPosition[2] / newPosition[3];
  this->computeDistance();
}


void vesCamera::dolly(double factor)
{
  if (factor <= 0.0) {
    return;
  }

  // dolly moves the camera towards the focus
  double d = this->m_distance/factor;

  this->m_position[0] = this->m_focalPoint[0] - d*this->m_directionOfProjection[0];
  this->m_position[1] = this->m_focalPoint[1] - d*this->m_directionOfProjection[1];
  this->m_position[2] = this->m_focalPoint[2] - d*this->m_directionOfProjection[2];
  this->computeDistance();
}


void vesCamera::roll(double angle)
{
  // rotate m_viewUp about the Direction of Projection
  vesMatrix4x4f m = makeRotationMatrix4x4(-deg2Rad(angle),
                                          this->m_directionOfProjection[0],
                                          this->m_directionOfProjection[1],
                                          this->m_directionOfProjection[2]);
  vesAffine3f t(m);
  vesVector3f newViewUp = t * this->m_viewUp;
  this->setViewUp(newViewUp);
}


void vesCamera::setWindowCenter(double x, double y)
{
  this->m_windowCenter[0] = x;
  this->m_windowCenter[1] = y;
}


void vesCamera::setClippingRange(float near, float far)
{
  this->m_clippingRange[0] = near;
  this->m_clippingRange[1] = far;
}


void vesCamera::orthogonalizeViewUp()
{
  // the orthogonalized m_viewUp is just the second row of the view matrix
  vesMatrix4x4f view = this->computeViewTransform();
  this->m_viewUp[0] = view(1, 0);
  this->m_viewUp[1] = view(1, 1);
  this->m_viewUp[2] = view(1, 2);
}


// This method must be called when the focal point or camera m_position changes
void vesCamera::computeDistance()
{
  double dx = this->m_focalPoint[0] - this->m_position[0];
  double dy = this->m_focalPoint[1] - this->m_position[1];
  double dz = this->m_focalPoint[2] - this->m_position[2];

  this->m_distance = sqrt(dx*dx + dy*dy + dz*dz);

  if (this->m_distance < 1e-20)
  {
    this->m_distance = 1e-20;
    vesVector3f vec = this->m_directionOfProjection;

    // recalculate m_focalPoint
    this->m_focalPoint[0] = this->m_position[0] + vec[0]*this->m_distance;
    this->m_focalPoint[1] = this->m_position[1] + vec[1]*this->m_distance;
    this->m_focalPoint[2] = this->m_position[2] + vec[2]*this->m_distance;
  }

  this->m_directionOfProjection[0] = dx/this->m_distance;
  this->m_directionOfProjection[1] = dy/this->m_distance;
  this->m_directionOfProjection[2] = dz/this->m_distance;

  this->computeViewPlaneNormal();
}


void vesCamera::computeViewPlaneNormal()
{
  this->m_viewPlaneNormal[0] = -this->m_directionOfProjection[0];
  this->m_viewPlaneNormal[1] = -this->m_directionOfProjection[1];
  this->m_viewPlaneNormal[2] = -this->m_directionOfProjection[2];
}


bool vesCamera::setRenderTarget(vesRenderTarget *renderTarget)
{
  // If none is given, use the default.
  if (!renderTarget && this->m_renderTargetStack.size() > 1) {

    this->m_removedRenderTargetStack.push_back(this->m_renderTargetStack.back());
    this->m_renderTargetStack.pop_back();

    return true;
  }
  else if(!renderTarget){
    return false;
  }

  RenderTargetStack::iterator itr;
  itr = std::find(this->m_renderTargetStack.begin(),
                  this->m_renderTargetStack.end(), renderTarget);

  if (itr == this->m_renderTargetStack.end()) {
    this->m_renderTargetStack.push_back(renderTarget)  ;
    return true;
  }
  else {
    return false;
  }
}


vesRenderTarget* vesCamera::renderTarget()
{
  return this->m_renderTargetStack.back();
}


const vesRenderTarget* vesCamera::renderTarget() const
{
  return this->m_renderTargetStack.back();
}


void vesCamera::clearRenderTargets(vesRenderState &renderState)
{
  for (size_t i = 0; i < this->m_removedRenderTargetStack.size(); ++i)  {
    this->m_removedRenderTargetStack.back()->remove(renderState);
  }

  this->m_removedRenderTargetStack.clear();
}


const vesSharedPtr<vesViewport> vesCamera::viewport() const
{
  return this->m_viewport;
}


vesSharedPtr<vesViewport> vesCamera::viewport()
{
  return this->m_viewport;
}


vesSharedPtr<vesRenderStage> vesCamera::renderStage()
{
  return this->m_renderStage;
}


const vesSharedPtr<vesRenderStage> vesCamera::renderStage() const
{
  return this->m_renderStage;
}


vesSharedPtr<vesRenderStage> vesCamera::getOrCreateRenderStage()
{
  if (!this->m_renderStage) {
    this->m_renderStage = vesRenderStage::Ptr(new vesRenderStage());
  }
  return this->m_renderStage;
}


vesMatrix4x4f vesCamera::modelViewMatrix()
{
  return this->computeViewTransform();
}


vesMatrix4x4f vesCamera::projectionMatrix()
{
  // Hard code -1, 1 for now.
  return this->computeProjectionTransform(
    this->m_viewport->aspect(), -1, 1);
}


void vesCamera::setRenderOrder(RenderOrder renderOrder, int renderOrderPriority)
{
  this->m_renderOrder = renderOrder;
  this->m_renderOrderPriority = renderOrderPriority;
}


vesCamera::RenderOrder vesCamera::renderOrder() const
{
  return this->m_renderOrder;
}


int vesCamera::renderOrderPriority() const
{
  return this->m_renderOrderPriority;
}


void vesCamera::setClearMask(unsigned int clearMask)
{
  this->m_clearMask = clearMask;
}


unsigned int vesCamera::clearMask() const
{
  return this->m_clearMask;
}


void vesCamera::setClearColor(const vesVector4f &clearColor)
{
  this->m_clearColor = clearColor;
}


vesVector4f vesCamera::clearColor()
{
  return this->m_clearColor;
}


const vesVector4f& vesCamera::clearColor() const
{
  return this->m_clearColor;
}

void vesCamera::setClearDepth(double depth)
{
  this->m_clearDepth = depth;
}


double vesCamera::clearDepth() const
{
  return this->m_clearDepth;
}


void vesCamera::accept(vesVisitor &visitor)
{
  visitor.visit(*this);
}


bool vesCamera::computeLocalToWorldMatrix(vesMatrix4x4f &matrix,
                                          vesVisitor &visitor)
{
  vesNotUsed(visitor);

  if (this->m_referenceFrame == Absolute) {
    matrix = this->modelViewMatrix();
  }
  else  {
    matrix = matrix * this->modelViewMatrix();
  }

  return true;
}


bool vesCamera::computeWorldToLocalMatrix(vesMatrix4x4f &matrix,
                                          vesVisitor &visitor)
{
  vesNotUsed(visitor);

  vesMatrix4x4f inverseMatrix = this->modelViewMatrix().inverse();

  if (this->m_referenceFrame == Absolute) {
    matrix  = inverseMatrix;
  }
  else {
    matrix = inverseMatrix * matrix;
  }

  return true;
}
