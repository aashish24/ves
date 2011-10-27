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
class vesActor;
class vesCamera;
class vesGroupNode;
class vesMaterial;
class vesTransformNode;
class vesVisitor;

class vesNode : public vesObject, public vesBoundingObject
{
public:
  vesNode();
  virtual ~vesNode();

  virtual void accept(vesVisitor &visitor);

  inline vesGroupNode* parent(){ return this->m_parent; }

  bool setParent(vesGroupNode *parent);

  inline void setIsOverlayNode(bool value) { this->m_isOverlayNode = value; }
  inline bool isOverlayNode() const { return this->m_isOverlayNode; }

  void setVisible(bool value);
  bool isVisible() const { return this->m_visible; }

  virtual vesGroupNode* asGroupNode() { return 0x0; }
  virtual const vesGroupNode* asGroupNode() const { return 0x0; }
  virtual vesTransformNode* asTransformNode() { return 0x0; }
  virtual const vesTransformNode* asTransformNode() const { return 0x0; }
  virtual vesCamera* asCamera() { return 0x0; }
  virtual const vesCamera* asCamera() const { return 0x0; }
  virtual vesActor* asActor() { return 0x0; }
  virtual const vesActor* asActor() const { return 0x0; }

  /// Traverse upwards.
  virtual void ascend(vesVisitor &visitor){}

  /// Traverse downwards.
  virtual void traverse(vesVisitor &visitor){}

protected:
  virtual void computeBounds();
  virtual void updateBounds(vesNode &child){}

  bool m_isOverlayNode;
  bool m_visible;
  bool m_boundsDirty;

  vesMaterial *m_material;
  vesGroupNode *m_parent;
};

#endif // __VESNODE_H
