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

#ifndef VESACTOR_H
#define VESACTOR_H

#include "vesTransformNode.h"

// VES includes
#include "vesSetGet.h"

// Forward declarations
class vesMapper;
class vesMaterial;
class vesMultitouchWidget;
class vesVisitor;

/// \class vesActor
///  \brief Defines an entity in the scene.
class vesActor : public vesTransformNode
{
public:
  vesTypeMacro(vesActor);

  vesActor();
  ~vesActor();

  /// Evaluate the transform associated with the vtkActor.
  /// \return The 4x4 matrix of the actor.
  vesMatrix4x4f modelViewMatrix();

  /// Mark the actor as a sensor (true denotes an actor that is a sensor).
  /// \param isSensor True to mark the actor as a sensor in the scene.
  void setSensor(bool isSensor) { m_sensor = isSensor; }

  /// Check if this actor instance is a sensor.
  /// \return true if the actor is a sensor.
  bool isSensor() const { return m_sensor; }

  void setWidget(vesSharedPtr<vesMultitouchWidget> widget);
  vesSharedPtr<vesMultitouchWidget> widget() { return this->m_widget; }
  const vesSharedPtr<vesMultitouchWidget> widget() const { return this->m_widget; }

  void setVisible(bool value);
  bool isVisible() const { return m_visible; }

  void setMapper(vesSharedPtr<vesMapper> mapper);
  vesSharedPtr<vesMapper> mapper() { return this->m_mapper; }
  const vesSharedPtr<vesMapper> mapper() const { return this->m_mapper; }

  virtual void accept(vesVisitor &visitor);
  virtual void ascend(vesVisitor &visitor);

  /// \copydoc vesTransformNode::computeLocalToWorldMatrix
  virtual bool computeLocalToWorldMatrix(vesMatrix4x4f &matrix,
                                         vesVisitor &visitor);

  /// \copydoc vesTransformNode::computeWorldToLocalMatrix
  virtual bool computeWorldToLocalMatrix(vesMatrix4x4f& matrix,
                                         vesVisitor& visitor);

protected:
  virtual void computeBounds();

  bool m_sensor;

  vesSharedPtr<vesMapper> m_mapper;
  vesSharedPtr<vesMultitouchWidget> m_widget;

private:
};

#endif // VESACTOR_H
