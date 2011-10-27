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

#include "vesCamera.h"

// VES includes
#include "vesStateAttributeBits.h"
#include "vesGMTL.h"
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

  this->FocalPoint[0] = 0.0;
  this->FocalPoint[1] = 0.0;
  this->FocalPoint[2] = 0.0;

  this->Position[0] = 0.0;
  this->Position[1] = 0.0;
  this->Position[2] = 1.0;

  this->ViewUp[0] = 0.0;
  this->ViewUp[1] = 1.0;
  this->ViewUp[2] = 0.0;

  this->DirectionOfProjection[0] = 0.0;
  this->DirectionOfProjection[1] = 0.0;
  this->DirectionOfProjection[2] = 0.0;

  this->ViewAngle = 30.0;
  this->UseHorizontalViewAngle = 0;

  this->ClippingRange[0] = 10.0f;
  this->ClippingRange[1] = 1010.0f;

  this->ParallelProjection = false;
  this->ParallelScale = 1.0;

  this->WindowCenter[0] = 0.0;
  this->WindowCenter[1] = 0.0;

  this->m_viewport = new vesViewport();

  this->m_renderStage = 0x0;

  this->m_renderOrder = PostRender;

  this->m_renderOrderPriority = 0;

  this->m_renderTargetStack.push_back(new vesRenderTarget());

  this->m_clearMask = vesStateAttributeBits::ColorBufferBit
    | vesStateAttributeBits::DepthBufferBit;
  this->m_clearColor = vesVector4f(1.0f, 1.0f, 1.0f, 1.0f);
  this->m_clearDepth = 1.0;

  this->ComputeDistance();
}


vesCamera::~vesCamera()
{
  // Delete only the first (default render target).
  assert(!this->m_renderTargetStack.empty());

  delete this->m_renderTargetStack[0];

  delete this->m_renderStage;
}


vesMatrix4x4f vesCamera::ComputeViewTransform()
{
  return vesLookAt (this->Position,
                    this->FocalPoint,
                    this->ViewUp);
}


vesMatrix4x4f vesCamera::ComputeProjectionTransform(float aspect,
                                                    float nearz,
                                                    float farz)
{
  vesMatrix4x4f matrix;

  // adjust Z-buffer range
  matrix[2][2] = (farz - nearz)/(1 - (-1));
  matrix[2][3] = (nearz*1 - farz*(-1))/(1 - (-1));


  if ( this->ParallelProjection)
  {
    // set up a rectangular parallelipiped

    double width = this->ParallelScale * aspect;
    double height = this->ParallelScale;

    double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
    double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
    double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
    double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

    vesMatrix4x4f ortho = vesOrtho( xmin, xmax, ymin, ymax,
                                    this->ClippingRange[0],
                                    this->ClippingRange[1] );
    return matrix * ortho;
  }
  else
  {
    // set up a perspective frustum
    double tmp = tan( deg2Rad( this->ViewAngle ) / 2. );
    double width;
    double height;
    if ( this->UseHorizontalViewAngle )
    {
      width = this->ClippingRange[0] * tmp;
      height = this->ClippingRange[0] * tmp / aspect;
    }
    else
    {
      width = this->ClippingRange[0] * tmp * aspect;
      height = this->ClippingRange[0] * tmp;
    }

    double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
    double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
    double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
    double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

    vesMatrix4x4f frustum = vesFrustum( xmin, xmax, ymin, ymax,
                                        this->ClippingRange[0],
                                        this->ClippingRange[1] );

    return matrix*frustum;
  }
}


// Rotate the camera about the view up vector centered at the focal point.
void vesCamera::Azimuth(double angle)
{
  vesVector3f fp = this->FocalPoint;
  vesVector3f vu = this->ViewUp;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(deg2Rad(angle), vu[0], vu[1], vu[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesMatrix4x4f t = t1 * t2 * t3;

  vesVector4f oldPosition(this->Position[0], this->Position[1], this->Position[2], 1);
  vesVector4f newPosition;
  gmtl::xform(newPosition, t, oldPosition);
  this->Position[0] = newPosition[0] / newPosition[3];
  this->Position[1] = newPosition[1] / newPosition[3];
  this->Position[2] = newPosition[2] / newPosition[3];
  this->ComputeDistance();
}


// Rotate the camera about the cross product of the negative of the
// direction of projection and the view up vector centered on the focal point.
void vesCamera::Elevation(double angle)
{
  vesMatrix4x4f view = this->ComputeViewTransform();
  vesVector3f axis;
  axis[0] = -view[0][0];
  axis[1] = -view[0][1];
  axis[2] = -view[0][2];
  vesVector3f fp = this->FocalPoint;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(-deg2Rad(angle), axis[0], axis[1], axis[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesMatrix4x4f t = t1 * t2 * t3;

  vesVector4f oldPosition(this->Position[0], this->Position[1], this->Position[2], 1);
  vesVector4f newPosition;
  gmtl::xform(newPosition, t, oldPosition);
  this->Position[0] = newPosition[0] / newPosition[3];
  this->Position[1] = newPosition[1] / newPosition[3];
  this->Position[2] = newPosition[2] / newPosition[3];
  this->ComputeDistance();
}


void vesCamera::Dolly(double factor)
{
  if (factor <= 0.0) {
    return;
  }

  // dolly moves the camera towards the focus
  double d = this->Distance/factor;

  this->Position[0] = this->FocalPoint[0] - d*this->DirectionOfProjection[0];
  this->Position[1] = this->FocalPoint[1] - d*this->DirectionOfProjection[1];
  this->Position[2] = this->FocalPoint[2] - d*this->DirectionOfProjection[2];
  this->ComputeDistance();
}


void vesCamera::Roll(double angle)
{
  // rotate ViewUp about the Direction of Projection
  vesMatrix4x4f t = makeRotationMatrix4x4(-deg2Rad(angle),
                                          this->DirectionOfProjection[0],
                                          this->DirectionOfProjection[1],
                                          this->DirectionOfProjection[2]);
  vesVector3f newViewUp;
  gmtl::xform(newViewUp, t, this->ViewUp);
  this->SetViewUp(newViewUp);
}


void vesCamera::SetWindowCenter(double x, double y)
{
  this->WindowCenter[0] = x;
  this->WindowCenter[1] = y;
}


void vesCamera::SetClippingRange(float near, float far)
{
  this->ClippingRange[0] = near;
  this->ClippingRange[1] = far;
}


void vesCamera::OrthogonalizeViewUp()
{
  // the orthogonalized ViewUp is just the second row of the view matrix
  vesMatrix4x4f view = this->ComputeViewTransform();
  this->ViewUp[0] = view[1][0];
  this->ViewUp[1] = view[1][1];
  this->ViewUp[2] = view[1][2];
}


// This method must be called when the focal point or camera position changes
void vesCamera::ComputeDistance()
{
  double dx = this->FocalPoint[0] - this->Position[0];
  double dy = this->FocalPoint[1] - this->Position[1];
  double dz = this->FocalPoint[2] - this->Position[2];

  this->Distance = sqrt(dx*dx + dy*dy + dz*dz);

  if (this->Distance < 1e-20)
  {
    this->Distance = 1e-20;
    vesVector3f vec = this->DirectionOfProjection;

    // recalculate FocalPoint
    this->FocalPoint[0] = this->Position[0] + vec[0]*this->Distance;
    this->FocalPoint[1] = this->Position[1] + vec[1]*this->Distance;
    this->FocalPoint[2] = this->Position[2] + vec[2]*this->Distance;
  }

  this->DirectionOfProjection[0] = dx/this->Distance;
  this->DirectionOfProjection[1] = dy/this->Distance;
  this->DirectionOfProjection[2] = dz/this->Distance;

  this->ComputeViewPlaneNormal();
}


void vesCamera::ComputeViewPlaneNormal()
{
  this->ViewPlaneNormal[0] = -this->DirectionOfProjection[0];
  this->ViewPlaneNormal[1] = -this->DirectionOfProjection[1];
  this->ViewPlaneNormal[2] = -this->DirectionOfProjection[2];
}


bool vesCamera::SetRenderTarget(vesRenderTarget *renderTarget)
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


vesRenderTarget* vesCamera::RenderTarget()
{
  return this->m_renderTargetStack.back();
}


const vesRenderTarget* vesCamera::RenderTarget() const
{
  return this->m_renderTargetStack.back();
}


void vesCamera::ClearRenderTargets(vesRenderState &renderState)
{
  for (size_t i = 0; i < this->m_removedRenderTargetStack.size(); ++i)  {
    this->m_removedRenderTargetStack.back()->remove(renderState);
  }

  this->m_removedRenderTargetStack.clear();
}


const vesViewport* vesCamera::viewport() const
{
  return this->m_viewport;
}


vesViewport* vesCamera::viewport()
{
  return this->m_viewport;
}


const vesRenderStage* vesCamera::renderStage() const
{
  return this->m_renderStage;
}


vesRenderStage* vesCamera::getOrCreateRenderStage()
{
  if (!this->m_renderStage) {
    this->m_renderStage = new vesRenderStage();
  }
  return this->m_renderStage;
}


vesMatrix4x4f vesCamera::modelViewMatrix()
{
  return this->ComputeViewTransform();
}


vesMatrix4x4f vesCamera::projectionMatrix()
{
  // Hard code -1, 1 for now.
  return this->ComputeProjectionTransform(
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
