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
/// \class vesActor
/// \ingroup ves
/// \brief Transform node that contains a drawable entity
///
/// vesActor is a placeholder transform node that contains a drawable entity.
/// One actor can contain only one drawable entity (mapper).
/// A mapper however can be set to multiple actors.
/// \see vesTransformNode vesMapper

#ifndef VESACTOR_H
#define VESACTOR_H

#include "vesTransformNode.h"

// VES includes
#include "vesSetGet.h"

// Forward declarations
class vesMapper;
class vesMaterial;
class vesVisitor;

class vesActor : public vesTransformNode
{
public:
  vesTypeMacro(vesActor);

  vesActor();
  ~vesActor();

  /// Evaluate the transform associated with the vtkActor.
  /// Return affine transformation for the actor.
  vesMatrix4x4f modelViewMatrix();

  /// Set mapper for the actor
  /// \see vesMapper
  void setMapper(vesSharedPtr<vesMapper> mapper);
  /// Get mapper of the actor
  /// \see vesMapper
  vesSharedPtr<vesMapper> mapper() { return this->m_mapper; }
  const vesSharedPtr<vesMapper> mapper() const { return this->m_mapper; }

  /// \copydoc vesTransformNode::accept()
  virtual void accept(vesVisitor &visitor);

  /// \copydoc vesTransformNode::ascend()
  virtual void ascend(vesVisitor &visitor);

  /// \copydoc vesTransformNode::computeLocalToWorldMatrix()
  virtual bool computeLocalToWorldMatrix(vesMatrix4x4f &matrix,
                                         vesVisitor &visitor);

  /// \copydoc vesTransformNode::computeWorldToLocalMatrix()
  virtual bool computeWorldToLocalMatrix(vesMatrix4x4f& matrix,
                                         vesVisitor& visitor);

protected:
  virtual void computeBounds();

  vesSharedPtr<vesMapper> m_mapper;

private:
};

#endif // VESACTOR_H
