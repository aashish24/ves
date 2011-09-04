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
#include "vesActor.h"

#include "vesMapper.h"
#include "vesShader.h"
#include "Painter.h"
#include "vsg/Shape/Shape.h"
#include "vsg/Shape/Appearance.h"

class vesActorInternal
{
};

vesActor::vesActor(vesShader *shader, vesMapper* mapper,
                   vesMultitouchWidget *widget)
{
  m_internal = new vesActorInternal();
  m_shape = new vsg::Shape();
  m_appearance = new vsg::Appearance();
  MFNode shaders;
  shaders.push_back(shader);
  m_appearance->set_shaders(shaders);
  m_shape->set_geometry(mapper);
  m_shape->set_appearance(m_appearance);
  m_mapper = mapper;        // This is used to make the actor visible again
  addShapeChild(m_shape);
  if(widget) {
    m_sensor = true;
    m_widget = widget;
  }
  else {
    m_sensor = false;
  }
  m_visible = true;
}

vesActor::~vesActor()
{
  delete m_internal;
  delete m_appearance;
  delete m_shape;
}

void vesActor::setColor(float r, float g, float b, float a)
{
  m_mapper->SetColor(r, g, b, a);
}

vesMatrix4x4f vesActor::eval()
{
  return this->Transform::eval();
}

bool vesActor::read()
{
  for (int i = 0; i < this->get_children().size(); ++i)
    this->get_children()[i]->read();

  return true;
}

void vesActor::render(Painter* render)
{
  render->Actor(this);
}

void vesActor::addShapeChild(vsg::Shape* shape)
{
  MFNode temp;
  temp.push_back(shape);
  set_children(temp);
}

void vesActor::computeBounds()
{
  m_shape->computeBounds();
  vesVector3f min = transformPoint3f(this->eval(), m_shape->get_min());
  vesVector3f max = transformPoint3f(this->eval(), m_shape->get_max());
  set_BBoxCenter(min, max);
  set_BBoxSize(min, max);
}

void vesActor::setVisible(bool value)
{
  if(value)
    m_shape->set_geometry(m_mapper);
  else
    m_shape->set_geometry(NULL);

  m_visible = value;
}

void vesActor::setTranslation(const vesVector3f& translation)
{
  set_translation(translation);
}

vesVector3f vesActor::translation() const
{
  return get_translation();
}

void vesActor::setRotation(const vesVector4f& rotation)
{
  set_rotation(rotation);
}

vesVector4f vesActor::rotation() const
{
  return get_rotation();
}
