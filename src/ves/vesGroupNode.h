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

#ifndef __VESGROUPNODE_H
#define __VESGROUPNODE_H

#include "vesNode.h"

// C/C++ includes
#include <list>

class vesGroupNode: public vesNode
{
public:
  vesGroupNode();
  virtual ~vesGroupNode();

  typedef std::list<vesNode*> Children;

  bool addChild   (vesNode *child);
  bool removeChild(vesNode *child);

  Children&       children()       { return this->m_children; }
  const Children& children() const { return this->m_children; }

  virtual void accept(vesVisitor &visitor);
  virtual void traverse(vesVisitor &visitor);

protected:
  void traverseChildrenAndUpdateBounds(vesVisitor &visitor);
  void traverseChildren(vesVisitor &visitor);

  virtual void updateBounds(vesNode &child);

  Children m_children;
};

#endif // __VESGROUPNODE_H
