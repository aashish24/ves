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


bool vesGroupNode::setVisible(bool value)
{
  // Make sure to call base class implementation first
  if(!vesNode::setVisible(value)) {
    return false;
  }

  Children::iterator itr = this->m_children.begin();
  for (; itr != this->m_children.end(); ++itr) {

    (*itr)->setVisible(value);
  }

  return true;
}


bool vesGroupNode::addChild(vesSharedPtr<vesNode> child)
{
  if (!child) {
    return false;
  }

  Children::iterator itr = this->m_children.begin();

  for (; itr != this->m_children.end(); ++itr) {
    if ( (*itr) == child ) {
      return false;
    }
  }

  child->setParent(this);

  this->m_children.push_back(child);

  this->setBoundsDirty(true);

  return true;
}


bool vesGroupNode::removeChild(vesSharedPtr<vesNode> child)
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


bool vesGroupNode::removeChild(vesNode *child)
{
  if (!child) {
    return false;
  }

  // \note:No check if the child really existed. This is for performance
  // reasons.

  // \note: Ensure that parent of this node is "this" group node.
  if (child->parent() == this) {

    // Remove the first matching node only.
    Children::const_iterator constItr = this->m_children.begin();
    for(; constItr != this->m_children.end(); ++constItr)
    {
      if((*constItr).get() == child)
      {
        this->m_children.remove(*constItr);
        break;
      }
    }

    this->setBoundsDirty(true);

    return true;
  }

  return false;
}


void vesGroupNode::accept(vesVisitor &visitor)
{
  visitor.visit(*this);
}


void vesGroupNode::traverse(vesVisitor &visitor)
{
  switch(visitor.type())
  {
  // Update
  case vesVisitor::UpdateVisitor:
    this->traverseChildrenAndUpdateBounds(visitor);
    break;
  case vesVisitor::CullVisitor:
    this->traverseChildren(visitor);
    break;
  default:
    // Do nothing.
    break;
  };
}


void vesGroupNode::traverseChildren(vesVisitor &visitor)
{
  Children::iterator itr = this->m_children.begin();
  if (visitor.mode() == vesVisitor::TraverseAllChildren) {
    for (; itr != this->m_children.end(); ++itr) {

      (*itr)->accept(visitor);
    }
  }
}


void vesGroupNode::traverseChildrenAndUpdateBounds(vesVisitor &visitor)
{
  this->computeBounds();

  Children::iterator itr = this->m_children.begin();

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

  // Since by now, we have updated the node bounds it is
  // safe to mark that bounds are no longer dirty anymore
  this->setBoundsDirty(false);
}


void vesGroupNode::updateBounds(vesNode &child)
{
  if (!this->boundsDirty()) {
    return;
  }

  // FIXME: This check should not be required and possibly is incorrect
  if (child.isOverlayNode()) {
    return;
  }

  // Make sure that child bounds are upto date
  child.computeBounds();

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
