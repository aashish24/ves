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

#ifndef VESVISITOR_H
#define VESVISITOR_H

#include "vesActor.h"

// Forward declarations
class vesMapper;
class vesMaterial;

class vesVisitor
{
public:
  enum TraversalMode
  {
    TraverseNone            = 0x1,
    TraverseParents         = 0x2,
    TraverseAllChildren     = 0x4,
    TraverseActiveChildren  = 0x8
  };

  enum VisitorType
  {
    ActorVisitor  = 0x1,
    UpdateVisitor = 0x2,
    EventVisitor  = 0x4,
    CullVisitor   = 0x8
  };

           vesVisitor(VisitorType type, TraversalMode=TraverseNone);
  virtual ~vesVisitor();

  void            pushActor(const vesActor& actor);
  void            popActor();
  vesActor*       actor();
  const vesActor* actor() const;

  void  pushModelViewMatrix(const vesMatrix4x4f& matrix);
  void  popModelViewMatrix ();

  void  pushProjectionMatrix(const vesMatrix4x4f& matrix);
  void  popProjectionMatrix ();

  vesMatrix4x4f modelViewMatrix();
  vesMatrix4x4f projectionMatrix();

  vesMatrix4x4f projection2DMatrix() { return this->m_projection2DMatrix; }
  vesMatrix4x4f setProjection2DMatrix(const vesMatrix4x4f& matrix) { this->m_projection2DMatrix = matrix; }

  inline void traverse(vesActor& actor)
  {
      if (m_traversalMode == TraverseParents) {
        actor.ascend(*this);
      }
      else if (m_traversalMode != TraverseNone) {
        actor.traverse(*this);
      }
  }

  virtual void visit(vesActor  &actor);

#if 0
  virtual void visit(vesMaterial& material);
#endif

  VisitorType   type(){ return this->m_visitorType; }
  TraversalMode mode(){ return this->m_traversalMode; }

protected:

  TraversalMode m_traversalMode;
  VisitorType   m_visitorType;

  vesMatrix4x4f m_projection2DMatrix;

  class vesInternal;
  vesInternal* m_internal;
};


#endif // VESVISITOR_H
