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

#ifndef __vsgGroupingNode_h
#define __vsgGroupingNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"

#include "vsg/Core/vsgNode.h"
#include "vsg/Grouping/vsgBoundedObject.h"

#include <list>

class vsgGroupingNode: public vsgNode, public vsgBoundedObject
{
public:
           vsgGroupingNode();
  virtual ~vsgGroupingNode();

  typedef std::list<vsgNode*> Children;

  bool addChild   (vsgNode *child);
  bool removeChild(vsgNode *child);

  Children&       children()       { return this->m_children; }
  const Children& children() const { return this->m_children; }


protected:
  Children m_children;
};

#endif // __vsgGroupingNode_h
