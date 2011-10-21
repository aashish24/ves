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

#include "vesActor.h"

// VES includes
#include "vesMapper.h"
#include "vesMaterial.h"
#include "vesVisitor.h"


vesActor::vesActor() : vesTransformNode(),
  m_sensor  (false),
  m_mapper  (0x0),
  m_widget  (0x0)
{
  // \todo: Create a default apperance.
}


vesActor::~vesActor()
{
}


vesMatrix4x4f vesActor::modelViewMatrix()
{
  return this->vesTransformNode::eval();
}


void vesActor::setVisible(bool value)
{
  this->m_visible = value;
}


void vesActor::setWidget(vesMultitouchWidget *widget)
{
  if (widget) {
    this->m_widget = widget;
  }
}


void vesActor::setMapper(vesMapper *mapper)
{
  if (mapper) {
    this->m_mapper = mapper;
    this->setBoundsDirty(true);
  }
}


void vesActor::setMaterial(vesMaterial *material)
{
  if (material) {
    this->m_material = material;
  }
}


void vesActor::accept(vesVisitor &visitor)
{
  visitor.visit(*this);
}


void vesActor::ascend(vesVisitor &visitor)
{
  // \todo: Implement this.
}


void vesActor::traverse(vesVisitor &visitor)
{
  Children::iterator itr = this->m_children.begin();

  switch(visitor.type())
  {
    // Update
    case vesVisitor::UpdateVisitor:
    {
      this->computeBounds();

      if (visitor.mode() == vesVisitor::TraverseAllChildren) {
        for (; itr != this->m_children.end(); ++itr) {

          (*itr)->accept(visitor);

          this->updateBounds(*(*itr));
        }
      }

      if (this->m_parent && this->boundsDirty()) {
        // Flag parents bounds dirty.
        this->m_parent->setBoundsDirty(true);
      }
      this->setBoundsDirty(false);

      break;
    }
    case vesVisitor::CullVisitor:
    {
      vesTransformNode::traverse(visitor);
      break;
    }
    default:
    {
      // Do nothing.
    }
  };
}


void vesActor::computeBounds()
{
  if (this->m_mapper && this->m_mapper->boundsDirty()) {
    this->m_mapper->computeBounds();

    vesVector3f min = transformPoint3f(this->eval(), this->m_mapper->boundsMinimum());
    vesVector3f max = transformPoint3f(this->eval(), this->m_mapper->boundsMaximum());

    this->setBounds(min, max);

    // Since now we have new internal bounds, we would have to
    // calculate whole bounds for this actor once again.
    this->setBoundsDirty(true);
  }
  else if (!this->m_mapper && this->boundsDirty()) {
    this->resetBounds();
  }
}
