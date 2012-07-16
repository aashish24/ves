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

#include  "vesNode.h"

// VES includes.
#include  "vesGroupNode.h"
#include "vesVisitor.h"

vesNode::vesNode() : vesBoundingObject(),
  m_visible (true),
  m_isOverlayNode(false),
  m_parent(0x0)
{
  this->setDirtyStateOff();
}


vesNode::~vesNode()
{
}


void vesNode::accept(vesVisitor &visitor)
{
  visitor.visit(*this);
}


void vesNode::setMaterial(vesSharedPtr<vesMaterial> material)
{
  if (material) {
    this->m_material = material;
  }
}


bool vesNode::setParent(vesGroupNode *parent)
{
  if (this->m_parent) {
    this->m_parent->removeChild(this);
  }

  this->m_parent = parent;

  return true;
}


void vesNode::setParentBoundsDirty(bool value)
{
  if(this->m_parent) {
    this->parent()->setBoundsDirty(value);
  }
}


bool vesNode::parentBoundsDirty()
{
  if(this->m_parent) {
    return this->parent()->boundsDirty();
  }

  return false;
}


bool vesNode::setVisible(bool value)
{
  // If node has a parent and if parent is not visible then at least now
  // this node cannot be made visible (visibilty is forced inherited).
  if(this->m_parent && !this->m_parent->isVisible() && value) {
    return false;
  }

  this->m_visible = value;

  return true;
}


void vesNode::computeBounds()
{
  if (this->boundsDirty()) {
    this->resetBounds();
  }
}
