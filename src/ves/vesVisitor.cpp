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

#include "vesVisitor.h"

#include "vesActor.h"
#include "vesGMTL.h"
#include "vesGroupNode.h"
#include "vesMapper.h"
#include "vesNode.h"
#include "vesTransformNode.h"

// C++ includes
#include <deque>
#include <vector>

class vesVisitor::vesInternal
{
public:

  std::deque <const vesActor*>      m_actorStack;
  std::vector<const vesMatrix4x4f*> m_modelViewMatrixStack;
  std::vector<const vesMatrix4x4f*> m_projectionMatrixStack;
};


vesVisitor::vesVisitor(VisitorType type, TraversalMode mode) :
  m_visitorType   (type),
  m_traversalMode (mode)
{
  this->m_internal = new vesInternal();
}


vesVisitor::~vesVisitor()
{
  delete this->m_internal;
}


void vesVisitor::pushActor(const vesActor& actor)
{
  this->m_internal->m_actorStack.push_back(&actor);
}


void vesVisitor::popActor()
{
  this->m_internal->m_actorStack.pop_back();
}


vesActor* vesVisitor::actor()
{
  return const_cast<vesActor*>(this->actor());
}


const vesActor* vesVisitor::actor() const
{
  return this->m_internal->m_actorStack.back();
}


void vesVisitor::pushModelViewMatrix(const vesMatrix4x4f &matrix)
{
  this->m_internal->m_modelViewMatrixStack.push_back(&matrix);
}


void vesVisitor::popModelViewMatrix()
{
  this->m_internal->m_modelViewMatrixStack.pop_back();
}


void vesVisitor::pushProjectionMatrix(const vesMatrix4x4f &matrix)
{
  this->m_internal->m_projectionMatrixStack.push_back(&matrix);
}


void vesVisitor::popProjectionMatrix()
{
  this->m_internal->m_projectionMatrixStack.pop_back();
}


vesMatrix4x4f vesVisitor::modelViewMatrix()
{
  vesMatrix4x4f matrix;
  size_t count = this->m_internal->m_modelViewMatrixStack.size();
  for (size_t i = 0; i < count ; ++i) {
    matrix *= *this->m_internal->m_modelViewMatrixStack[i];
  }

  return matrix;
}


vesMatrix4x4f vesVisitor::projectionMatrix()
{
  vesMatrix4x4f matrix;
  size_t count = this->m_internal->m_projectionMatrixStack.size();
  for (size_t i = 0; i < count; ++i) {
    matrix *= *this->m_internal->m_projectionMatrixStack [i];
  }

  return matrix;
}


void vesVisitor::visit(vesNode &node)
{
  this->traverse(node);
}


void vesVisitor::visit(vesGroupNode & groupNode)
{
  this->visit(static_cast<vesNode&>(groupNode));
}


void vesVisitor::visit(vesTransformNode &transformNode)
{
  this->visit(static_cast<vesGroupNode&>(transformNode));
}


void vesVisitor::visit(vesActor &actor)
{
  this->visit(static_cast<vesTransformNode&>(actor));
}
