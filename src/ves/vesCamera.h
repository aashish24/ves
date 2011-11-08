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
  vesTypeMacro(vesCamera);

  vesCamera();
  virtual  ~vesCamera();

  enum RenderOrder
  {
    PreRender = 0,
    PostRender,
    NestedRender
  };

  inline void setUseHorizontalViewAngle(bool value)
    { this->m_useHorizontalViewAngle = value; }
  inline bool useHorizontalViewAngle() const
    { return this->m_useHorizontalViewAngle; }

  inline void setViewPlaneNormal(const vesVector3f &viewPlaneNormal)
    { this->m_viewPlaneNormal = viewPlaneNormal; }
  inline vesVector3f viewPlaneNormal() { return this->m_viewPlaneNormal; }
  inline const vesVector3f& viewPlaneNormal() const { return this->m_viewPlaneNormal; }

  inline void setViewAngle(float viewAngle) { this->m_viewAngle = viewAngle; }
  float viewAngle() const { return this->m_viewAngle; }

  inline void setPosition(const vesVector3f &m_position)
    { this->m_position = m_position; this->computeDistance(); }
  vesVector3f position() { return this->m_position; }
  const vesVector3f& position() const { return this->m_position; }

  inline void setFocalPoint(const vesVector3f &focalPoint)
    { this->m_focalPoint = focalPoint; }
  vesVector3f focalPoint() { return this->m_focalPoint; }
  const vesVector3f& focalPoint() const { return this->m_focalPoint; }

  inline void setViewUp(const vesVector3f &viewUp)
    { this->m_viewUp = viewUp; }
  vesVector3f viewUp() { return this->m_viewUp; }
  const vesVector3f& viewUp() const { return this->m_viewUp; }

  inline void setParallelScale(float parallelScale)
    { this->m_parallelScale = parallelScale; }
  float parallelScale() const { return this->m_parallelScale; }

  inline void setParallelProjection(bool value)
    { this->m_parallelProjection = value; }
  bool parallelProjection() const { return this->m_parallelProjection; }

  void azimuth(double angle);
  void elevation(double angle);
  void dolly(double factor);
  void roll(double angle);
  void orthogonalizeViewUp();

  vesMatrix4x4f computeViewTransform();
  vesMatrix4x4f computeProjectionTransform(float aspect, float near, float far);
  void setWindowCenter(double x, double y);
  void setClippingRange(float near, float far);
  void reset();

  bool setRenderTarget(vesRenderTarget *renderTarget);
  vesRenderTarget* renderTarget();
  const vesRenderTarget* renderTarget() const;
  void clearRenderTargets(vesRenderState &renderState);

  const vesSharedPtr<vesViewport> viewport() const;
  vesSharedPtr<vesViewport> viewport();

  vesSharedPtr<vesRenderStage> renderStage();
  const vesSharedPtr<vesRenderStage> renderStage() const;
  vesSharedPtr<vesRenderStage> getOrCreateRenderStage();

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
  void computeDistance();
  void computeViewPlaneNormal();

  typedef std::vector<vesRenderTarget*> RenderTargetStack;

  float         m_viewAngle;
  bool          m_useHorizontalViewAngle;
  vesVector3f   m_viewPlaneNormal;
  vesVector3f   m_position, m_focalPoint,m_viewUp;
  float         m_parallelScale;
  float         m_clippingRange[2];
  float         m_distance;
  vesVector3f   m_directionOfProjection;
  double        m_windowCenter[2];
  bool          m_parallelProjection;

  vesSharedPtr<vesViewport> m_viewport;
  vesSharedPtr<vesRenderStage> m_renderStage;

  RenderOrder m_renderOrder;
  int         m_renderOrderPriority;

  RenderTargetStack m_renderTargetStack;
  RenderTargetStack m_removedRenderTargetStack;

  unsigned int m_clearMask;
  vesVector4f m_clearColor;
  double m_clearDepth;
};
#endif //__vesCamera_h
