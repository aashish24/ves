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

#ifndef __VESNODE_H
#define __VESNODE_H

#include "vesBoundingObject.h"
#include "vesObject.h"

// Forward declarations.
class vesMaterial;
class vesGroupNode;
class vesVisitor;

class vesNode : public vesObject, public vesBoundingObject
{
public:
  vesNode();
  virtual ~vesNode();

  virtual void accept(vesVisitor &visitor){}

  inline vesGroupNode* parent(){ return this->m_parent; }

  bool setParent(vesGroupNode *parent);

  inline void setIsOverlayNode(bool value) { this->m_isOverlayNode = value; }
  inline bool isOverlayNode() const { return m_isOverlayNode; }

  virtual void traverse(vesVisitor &visitor){}

protected:
  virtual void updateBounds(vesNode &child){}

  bool m_isOverlayNode;
  bool m_visible;
  bool m_boundsDirty;

  vesMaterial *m_material;
  vesGroupNode *m_parent;
};

#endif // __VESNODE_H
