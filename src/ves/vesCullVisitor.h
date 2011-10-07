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

#ifndef VESCULLVISITOR_H
#define VESCULLVISITOR_H

// Base class
#include "vesVisitor.h"

// Forward declarations
class vesRenderStage;

class vesCullVisitor : public vesVisitor
{
public:
  vesCullVisitor(TraversalMode mode=TraverseAllChildren) :
    vesVisitor    (CullVisitor, mode),
    m_renderStage (0x0)
  {
  }


  ~vesCullVisitor()
  {
  }


  void setRenderStage(vesRenderStage *renderStage)
  {
    if (renderStage && this->m_renderStage != renderStage) {
      this->m_renderStage = renderStage;
    }
  }


  vesRenderStage* renderStage()
  {
    return this->m_renderStage;
  }


  const vesRenderStage* renderStage() const
  {
    return this->m_renderStage;
  }


  virtual void visit(vesActor  &actor);


protected:

  void addGeometryAndStates(vesMapper *mapper, vesMaterial *material,
                            const vesMatrix4x4f &modelViewMatrix,
                            const vesMatrix4x4f& projectionMatrix,
                            float depth);

  vesRenderStage *m_renderStage;
};

#endif // VESCULLVISITOR_H
