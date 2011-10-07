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

# include "vsgGroupingNode.h"


vsgGroupingNode::vsgGroupingNode()
{
}


vsgGroupingNode::~vsgGroupingNode()
{
}


bool vsgGroupingNode::addChild(vsgNode *child)
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

  return true;
}


bool vsgGroupingNode::removeChild(vsgNode *child)
{
  if (!child) {
    return false;
  }

  // \note:No check if the child really existed. This is for performance
  // reasons.

  // \note: Ensure that parent of this node is "this" group node.
  if (child->parent() == this) {
    this->m_children.remove(child);

    return true;
  }

  return false;
}
