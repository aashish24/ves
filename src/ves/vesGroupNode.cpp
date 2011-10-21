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

#include "vesGroupNode.h"

// VES includes.
#include "vesVisitor.h"

vesGroupNode::vesGroupNode() : vesNode()
{
}


vesGroupNode::~vesGroupNode()
{
}


bool vesGroupNode::addChild(vesNode *child)
{
  if (!child) {
    return false;
  }

  Children::iterator itr = this->m_children.begin();

  for (itr; itr != this->m_children.end(); ++itr) {
    if ( (*itr) == child ) {
      return false;
    }
  }

  child->setParent(this);

  this->m_children.push_back(child);

  this->setBoundsDirty(true);

  return true;
}


bool vesGroupNode::removeChild(vesNode *child)
{
  if (!child) {
    return false;
  }

  // \note:No check if the child really existed. This is for performance
  // reasons.

  // \note: Ensure that parent of this node is "this" group node.
  if (child->parent() == this) {
    this->m_children.remove(child);

    this->setBoundsDirty(true);

    return true;
  }

  return false;
}


void vesGroupNode::traverse(vesVisitor &visitor)
{
  Children::iterator itr = this->m_children.begin();

  if (visitor.mode() == vesVisitor::TraverseAllChildren) {
    for (; itr != this->m_children.end(); ++itr) {
      (*itr)->accept(visitor);
    }
  }
  else if (visitor.mode() == vesVisitor::TraverseActiveChildren) {
    // \todo: Implement this.
  }
}


void vesGroupNode::updateBounds(vesNode &child)
{
  if (!this->boundsDirty()) {
    return;
  }

  if (child.isOverlayNode()) {
    return;
  }

  vesVector3f min = child.boundsMinimum();
  vesVector3f max = child.boundsMaximum();

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
