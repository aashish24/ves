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

#include "vsg/Grouping/Transform.h"

#include "vesGMTL.h"

// Forward declarations
class vesMapper;
class vesMaterial;
class vesMultitouchWidget;
class vesVisitor;

/*!
    \class vesActor
    \brief Defines an entity in the scene.
*/

class vesActor : public vsg::Transform
{
public:
   vesActor();
  ~vesActor();

  /*!
      Evaluate the transform associated with the vtkActor.
      \return The 4x4 matrix of the actor.
  */
  vesMatrix4x4f modelViewMatrix();

  /*!
      Set the translation for the actor.
      \param translation Translation of the actor from the origin.
  */
  void setTranslation(const vesVector3f &translation);

  /*!
      Get the translation for the actor.
      \return The translation of the actor from the origin.
  */
  vesVector3f translation() const;

  /*!
      Set the rotation for the actor.
      \param rotation Rotation of the actor.
  */
  void setRotation(const vesVector4f &rotation);

  /** Get the rotation for the actor.
   * \return The rotation of the actor from the origin.
   */
  vesVector4f rotation() const;

  /*!
      Mark the actor as a sensor (true denotes an actor that is a sensor).
      \param isSensor True to mark the actor as a sensor in the scene.
  */
  void setSensor(bool isSensor) { m_sensor = isSensor; }

  /*!
      Check if this actor instance is a sensor.
      \return true if the actor is a sensor.
  */
  bool isSensor() const { return m_sensor; }

  void                       setWidget(vesMultitouchWidget *widget);
  vesMultitouchWidget*       widget()       { return this->m_widget; }
  const vesMultitouchWidget* widget() const { return this->m_widget; }

  void setVisible(bool value);
  bool isVisible() const { return m_visible; }

  void             setMapper(vesMapper *mapper);
  vesMapper*       mapper()       { return this->m_mapper; }
  const vesMapper* mapper() const { return this->m_mapper; }

  void                setMaterial(vesMaterial *material);
  vesMaterial*        material()       { return this->m_material; }
  const vesMaterial*  material() const { return this->m_material; }


  virtual void accept(vesVisitor &visitor);

  virtual void ascend(vesVisitor &visitor);

  virtual void traverse(vesVisitor &visitor);

  void setIsOverlayActor(bool value) { this->m_isOverlayActor = value; }
  bool isOverlayActor() const { return m_isOverlayActor; }

protected:

  virtual void updateBounds(vesActor *child);
  virtual void computeBounds();

  bool                  m_sensor;
  bool                  m_visible;
  bool                  m_boundsDirty;
  bool                  m_isOverlayActor;

  vesMapper            *m_mapper;
  vesMaterial          *m_material;
  vesMultitouchWidget  *m_widget;


private:
};

#endif // VESACTOR_H
