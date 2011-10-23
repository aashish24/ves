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
#ifndef __vesCamera_h
#define __vesCamera_h

#include "vesTransformNode.h"

// VES includes
#include "vesGMTL.h"
#include "vesRenderTarget.h"
#include "vesSetGet.h"

// C/C++ includes
#include <vector>

// Forward declarations
class vesRenderStage;
class vesRenderState;
class vesViewport;
class vesVisitor;

class vesCamera: public vesTransformNode
{
public:
  vesCamera();
  virtual  ~vesCamera();

  enum RenderOrder
  {
    PreRender = 0,
    PostRender,
    InOrder
  };

  vesSetGetMacro(UseHorizontalViewAngle,bool)
  vesSetGetMacro(ViewPlaneNormal,vesVector3f)
  vesSetGetMacro(ViewAngle, float)
  void SetPosition(vesVector3f pos) { Position = pos; ComputeDistance(); }
  vesVector3f GetPosition() { return Position; }
  vesSetGetMacro(FocalPoint, vesVector3f)
  vesSetGetMacro(ViewUp, vesVector3f)
  vesSetGetMacro(ParallelScale, float)
  vesSetGetMacro(ParallelProjection, bool)

  void Azimuth(double angle);
  void Elevation(double angle);
  void Dolly(double factor);
  void Roll(double angle);
  void OrthogonalizeViewUp();

  vesMatrix4x4f ComputeViewTransform();
  vesMatrix4x4f ComputeProjectionTransform(float aspect, float near, float far);
  void SetWindowCenter(double x, double y);
  void SetClippingRange(float near, float far);
  void Reset();

  bool SetRenderTarget(vesRenderTarget *renderTarget);
  vesRenderTarget* RenderTarget();
  const vesRenderTarget* RenderTarget() const;
  void ClearRenderTargets(vesRenderState &renderState);

  const vesViewport* viewport() const;
  vesViewport* viewport();

  const vesRenderStage* renderStage() const;
  vesRenderStage* getOrCreateRenderStage();

  void setRenderOrder(RenderOrder renderOrder, int renderOrderPriority=0);
  RenderOrder renderOrder() const;
  int renderOrderPriority() const;

  void setClearMask(unsigned int clearMask);
  unsigned int clearMask() const;

  void setClearColor(const vesVector4f &clearColor);
  vesVector4f clearColor();
  const vesVector4f& clearColor() const;

  void setClearDepth(double depth);
  double clearDepth() const;

  virtual vesMatrix4x4f modelViewMatrix();
  virtual vesMatrix4x4f projectionMatrix();

  virtual void accept(vesVisitor &visitor);

protected:
  void ComputeDistance();
  void ComputeViewPlaneNormal();

  typedef std::vector<vesRenderTarget*> RenderTargetStack;

  float         ViewAngle;
  bool          UseHorizontalViewAngle;
  vesVector3f   ViewPlaneNormal;
  vesVector3f   Position, FocalPoint,ViewUp;
  float         ParallelScale;
  float         ClippingRange[2];
  float         Distance;
  vesVector3f   DirectionOfProjection;
  double        WindowCenter[2];
  bool          ParallelProjection;

  vesViewport *m_viewport;
  vesRenderStage *m_renderStage;

  RenderOrder m_renderOrder;
  int         m_renderOrderPriority;

  RenderTargetStack m_renderTargetStack;
  RenderTargetStack m_removedRenderTargetStack;

  unsigned int m_clearMask;
  vesVector4f m_clearColor;
  double m_clearDepth;
};
#endif //__vesCamera_h
