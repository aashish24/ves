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

// --------------------------------------------------------------------includes
#include "Appearance.h"
#include "Shape.h"
#include "vesMapper.h"
#include "vesShader.h"
#include "vesSetGet.h"
#include "Painter.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
class vesActorInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vesActor::vesActor(vesShader *shader,vesMapper* mapper,vesMultitouchWidget *widget)
{
  this->Internal = new vesActorInternal();
  this->_shape = new vsg::Shape();
  this->_appearance = new vsg::Appearance();
  MFNode shaders;
  shaders.push_back(shader);
  this->_appearance->set_shaders(shaders);
  this->_shape->set_geometry(mapper);
  this->_shape->set_appearance(this->_appearance);
  this->Mapper = mapper;        // This is used to make the actor visible again
  AddShapeChild(this->_shape);
  if(widget)
  {
    this->Sensor = true;
    this->Widget = widget;
    //std::cout<< "Adding widget interactor" <<std::endl;
  }
  else
  {
    this->Sensor = false;
  }
  this->Visible = true;
}


// -----------------------------------------------------------------------destr
vesActor::~vesActor()
{
  delete this->Internal;
  delete this->_appearance;
  delete this->_shape;
}

// ----------------------------------------------------------------------public
vesMapper* vesActor::GetMapper()
{
  return this->Mapper;
}

// ----------------------------------------------------------------------public
void vesActor::SetColor(float r, float g, float b, float a)
{
  this->Mapper->SetColor(r, g, b, a);
}

// ----------------------------------------------------------------------public
vesMatrix4x4f vesActor::Eval()
{
  return this->Transform::eval();
}

// ----------------------------------------------------------------------public
bool vesActor::Read()
{
  //std::cout << "Read: Actor" <<std::endl;
  for (int i = 0; i < this->get_children().size(); ++i)
  {
    this->get_children()[i]->Read();
  }
  return true;
}

// ----------------------------------------------------------------------public
void vesActor::Render(Painter* render)
{
  render->Actor(this);
}

// ----------------------------------------------------------------------public
void vesActor::AddShapeChild(vsg::Shape* shape)
{
  MFNode temp;
  temp.push_back(shape);
  set_children(temp);
}

// ----------------------------------------------------------------------public
void vesActor::ComputeBounds()
{
  _shape->ComputeBounds();
  vesVector3f min = transformPoint3f(this->Eval(), _shape->get_min());
  vesVector3f max = transformPoint3f(this->Eval(), _shape->get_max());
  set_BBoxCenter(min,max);
  set_BBoxSize(min,max);
}

// ----------------------------------------------------------------------public
bool vesActor::SetVisible(bool value)
{
  if(value)
  {
    this->_shape->set_geometry(this->Mapper);
  }
  else
  {
    this->_shape->set_geometry(NULL);
  }
  this->Visible = value;
  return true;
}

// ----------------------------------------------------------------------public
bool vesActor::isVisible()
{
  return true;
}

void vesActor::SetTranslation(vesVector3f translation)
{
  set_translation(translation);
}

void vesActor::SetRotation(vesVector4f rotation)
{
  set_rotation(rotation);
}

vesVector3f vesActor::GetTranslation()
{
  return get_translation();
}

vesVector4f vesActor::GetRotation()
{
  return get_rotation();
}
