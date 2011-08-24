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
// .NAME vesActor - Defines a entity in the scene
// .SECTION Description
// vesActor

#ifndef __vesActor_h
#define __vesActor_h
// --------------------------------------------------------------------includes
#include "vesGMTL.h"
#include "vesShaderProgram.h"
#include "vsg/Grouping/Transform.h"
#include "vsg/Shape/Shape.h"
#include "vsg/Shape/Appearance.h"
#include "vesSetGet.h"
#include <list>

// -----------------------------------------------------------------pre-defines
class vesActorInternal;
class vesShader;
class vesMapper;
class vesMultitouchWidget;

// -----------------------------------------------------------------------class
class vesActor : public vsg::Transform
{
public:
  // ............................................................public-methods
  vesActor(vesShader* shader,vesMapper* mapper,vesMultitouchWidget *widget=0);
  ~vesActor();

  vesMatrix4x4f Eval();
  bool Read();
  void ComputeBounds();
  void Render(Painter* render);
  void SetTranslation(vesVector3f translation);
  void SetRotation(vesVector4f rotation);
  vesVector3f GetTranslation();
  vesVector4f GetRotation();
  vesSetGetMacro(Sensor,bool)
  vesSetGetMacro(Widget,vesMultitouchWidget*);
  bool SetVisible(bool value);
  bool isVisible();
  vesMapper* GetMapper();
  void SetColor(float r, float g, float b, float a);

protected:
  // ...........................................................protected-ivars
  vsg::Appearance *_appearance;
  vsg::Shape *_shape;
  bool Sensor;
  vesMultitouchWidget* Widget;
  vesMapper *Mapper;
  bool Visible;
protected:
  //BTX
  // .......................................................................BTX
  void AddShapeChild(vsg::Shape* shape);
private:
  vesActorInternal *Internal;
  //ETX
  // .......................................................................ETX
};

#endif // __vesActor_h
