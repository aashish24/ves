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
#include "vesVisitor.h"

#include "vesMaterial.h"

class vesActor::vesActorInternal
{
};


vesActor::vesActor()
{
  this->m_sensor      = false;
  this->m_visible     = true;
  this->m_mapper      = 0x0;
  this->m_material    = 0x0;
  this->m_widget      = 0x0;
  this->m_internal    = new vesActorInternal();

  // \todo: Create a default apperance.
}


vesActor::~vesActor()
{
  delete this->m_internal; this->m_internal = 0x0;
}


vesMatrix4x4f vesActor::modelViewMatrix()
{
  return this->Transform::eval();
}


void vesActor::computeBounds()
{
  if (this->m_mapper) {
    this->m_mapper->computeBounds();
    vesVector3f min = transformPoint3f(this->eval(), this->m_mapper->get_min());
    vesVector3f max = transformPoint3f(this->eval(), this->m_mapper->get_max());
    set_BBoxCenter(min, max);
    set_BBoxSize(min, max);
  }
}


void vesActor::setVisible(bool value)
{
  this->m_visible = value;
}


void vesActor::setTranslation(const vesVector3f& translation)
{
//  set_translation(translation);
}

vesVector3f vesActor::translation() const
{
//  return get_translation();
}


void vesActor::setRotation(const vesVector4f& rotation)
{
//  set_rotation(rotation);
}


vesVector4f vesActor::rotation() const
{
//  return get_rotation();
}


void vesActor::setWidget(vesMultitouchWidget *widget)
{
  if (widget) {
    this->m_widget = widget;
  }
}


void vesActor::setMapper(vesMapper *mapper)
{
  if (mapper) {
    this->m_mapper = mapper;
  }
}


void vesActor::setMaterial(vesMaterial *material)
{
  if (material) {
    this->m_material = material;
  }
}


void vesActor::accept(vesVisitor &visitor)
{
  if (this->m_material) {
    this->m_material->accept(visitor);
  }

  if (this->m_mapper) {
    this->m_mapper->accept(visitor);
  }


  this->traverse(visitor);
}


void vesActor::ascend(vesVisitor &visitor)
{
  // \todo: Implement this.
}


void vesActor::traverse(vesVisitor &visitor)
{
  Children::iterator itr = this->m_children.begin();

  if (visitor.mode() == vesVisitor::TraverseAllChildren) {
    for (itr; itr != this->m_children.end(); ++itr) {
      (*itr)->accept(visitor);
    }
  }
  else if (visitor.mode() == vesVisitor::TraverseActiveChildren) {
    // \todo: Implement this.
  }
}
