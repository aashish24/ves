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

#ifndef __vesActor_h
#define __vesActor_h

#include "vsg/Grouping/Transform.h"

#include "vsg/Shape/Appearance.h"

#include "vesGMTL.h"

namespace vsg
{
class Shape;
class Appearance;
}

class vesActorInternal;
class vesShader;
class vesMapper;
class vesMultitouchWidget;

/**
 * \class vesActor
 * \brief Defines an entity in the scene.
 *
 * The vesActor class defines an entity in the scene, and aims to provide API
 * analagous to the vtkActor classes.
 */

class vesActor : public vsg::Transform
{
public:
  /**
   * Constructor.
   * \param shader The shader object to render the item with.
   * \param mapper The mapper to use to render the item with.
   * \param widget The widget that might be associated with the item.
   */
  vesActor(vesMapper* mapper,
           vesMultitouchWidget *widget = 0);

  ~vesActor();

  /**
   *
   */
  vsg::Appearance* appearance()
  {
    return m_appearance;
  }

  /**
   * Evaluate the transform associated with the vtkActor.
   * \return The 4x4 matrix of the actor.
   */
  vesMatrix4x4f eval();

  /** Read the current actor. */
  bool read();

  /** Compute the bounds of the actor. */
  void computeBounds();

  /** Render the actor. */
  void render(Painter* render);

  /** Set the translation for the actor.
   * \param translation Translation of the actor from the origin.
   */
  void setTranslation(const vesVector3f& translation);

  /** Get the translation for the actor.
   * \return The translation of the actor from the origin.
   */
  vesVector3f translation() const;

  /** Set the rotation for the actor.
   * \param rotation Rotation of the actor.
   */
  void setRotation(const vesVector4f& rotation);

  /** Get the rotation for the actor.
   * \return The rotation of the actor from the origin.
   */
  vesVector4f rotation() const;

  /** Mark the actor as a sensor (true denotes an actor that is a sensor).
   * \param isSensor True to mark the actor as a sensor in the scene.
   */
  void setSensor(bool isSensor) { m_sensor = isSensor; }

  /** Check if this actor instance is a sensor.
   * \return true if the actor is a sensor.
   */
  bool isSensor() const { return m_sensor; }

  void setWidget(vesMultitouchWidget* widget) { m_widget = widget; }
  vesMultitouchWidget* widget() { return m_widget; }
  const vesMultitouchWidget* widget() const { return m_widget; }

  void setVisible(bool value);
  bool isVisible() const { return m_visible; }

  vesMapper* mapper() { return m_mapper; }
  const vesMapper* mapper() const { return m_mapper; }

  /**
   * Set the color of the actor.
   * \param r Red component.
   * \param g Green component.
   * \param b Blue component.
   * \param a Alpha compoent.
   */
  void setColor(float r, float g, float b, float a);

protected:
  vsg::Appearance *m_appearance;
  vsg::Shape *m_shape;
  bool m_sensor;
  vesMultitouchWidget* m_widget;
  vesMapper *m_mapper;
  bool m_visible;

  void addShapeChild(vsg::Shape* shape);

private:
  vesActorInternal *m_internal;
};

#endif // __vesActor_h
