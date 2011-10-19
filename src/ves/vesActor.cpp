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


vesActor::vesActor() : vsg::Transform(),
  m_sensor  (false),
  m_visible (true),
  m_mapper  (0x0),
  m_material(0x0),
  m_widget  (0x0),
  m_isOverlayActor(false)
{
  // \todo: Create a default apperance.
}


vesActor::~vesActor()
{
}


vesMatrix4x4f vesActor::modelViewMatrix()
{
  return this->Transform::eval();
}


void vesActor::updateBounds(vesActor *child)
{
  if (!this->boundsDirty()) {
    return;
  }

  if (!child || child->isOverlayActor()) {
    return;
  }

  vesVector3f min = child->boundsMinimum();
  vesVector3f max = child->boundsMaximum();

  min = transformPoint3f(this->eval(), min);
  max = transformPoint3f(this->eval(), max);

  child->setBounds(min, max);

  for (int i = 0; i < 3; ++i) {
    if (max[i] > this->m_boundsMaximum[i]) {
      this->m_boundsMaximum[i] = max[i];
    }

    if (min[i] < this->m_boundsMinimum[i]) {
      this->m_boundsMinimum[i] = min[i];
    }
  }

  // Now update the bounds, bounds size and center.
  this->setBounds(this->m_boundsMinimum, this->m_boundsMaximum);
}


void vesActor::setVisible(bool value)
{
  this->m_visible = value;
}


void vesActor::setTranslation(const vesVector3f& translation)
{
  set_translation(translation);
  this->setBoundsDirty(true);
}


vesVector3f vesActor::translation() const
{
  return get_translation();
}


void vesActor::setRotation(const vesVector4f& rotation)
{
  set_rotation(rotation);
  this->setBoundsDirty(true);
}


vesVector4f vesActor::rotation() const
{
  return get_rotation();
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
  this->traverse(visitor);
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

          visitor.visit(*(static_cast<vesActor*>(*itr)));

          this->updateBounds(static_cast<vesActor*>(*itr));
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
      // Cull
      int i=0;
      if (visitor.mode() == vesVisitor::TraverseAllChildren) {
        for (; itr != this->m_children.end(); ++itr) {
          visitor.visit(*(static_cast<vesActor*>(*itr)));
        }
      }
      else if (visitor.mode() == vesVisitor::TraverseActiveChildren) {
        // \todo: Implement this.
      }

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
