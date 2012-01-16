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

#ifndef __VESTRANSFORMNODE_H
#define __VESTRANSFORMNODE_H

#include "vesGroupNode.h"

// VES includes
#include "vesSetGet.h"

class vesTransformNode : public vesGroupNode
{
public:
  vesTypeMacro(vesTransformNode);

  enum ReferenceFrame
  {
    Relative = 0,
    Absolute = 1
  };

  vesTransformNode();
  virtual ~vesTransformNode();

  void setCenter(const vesVector3f &center);
  const vesVector3f& center() const;

  void setRotation(const vesVector4f &rotation);
  const vesVector4f& rotation() const;

  void setScale(const vesVector3f &scale);
  const vesVector3f& scale() const;

  void setScaleOrientation(const vesVector4f &scaleOrientation);
  const vesVector4f& scaleOrientation() const;

  void setTranslation(const vesVector3f &translation);
  const vesVector3f& translation() const;

  bool setReferenceFrame(ReferenceFrame referenceFrame);
  ReferenceFrame referenceFrame() const;

  vesMatrix4x4f matrix();

  virtual vesTransformNode* asTransformNode() { return this; }
  virtual const vesTransformNode* asTransformNode() const { return this; }

  virtual void accept(vesVisitor &visitor);

  /// \brief Compute local to world matrix
  virtual bool computeLocalToWorldMatrix(vesMatrix4x4f& matrix,
                                         vesVisitor& visitor);

  /// \brief Compute world to local matrix
  virtual bool computeWorldToLocalMatrix(vesMatrix4x4f& matrix,
                                         vesVisitor& visitor);

protected:
  void updateBounds(vesNode &child);

  void setInternals();

  vesMatrix4x4f computeTransform();

  vesVector3f m_center;
  vesVector4f m_rotation;
  vesVector3f m_scale;
  vesVector4f m_scaleOrientation;
  vesVector3f m_translation;

  ReferenceFrame m_referenceFrame;

private:
  class vesInternal;
  vesInternal *m_internal;
};

#endif // __VESTRANSFORMNODE_H
