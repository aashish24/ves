/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesActor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesActor - Defines a entity in the scene
// .SECTION Description
// vesActor

#ifndef __vesActor_h
#define __vesActor_h
// --------------------------------------------------------------------includes
#include "vesGMTL.h"
#include "vesShaderProgram.h"
#include "vsg/Grouping/Transform.h"
#include "Shape.h"
#include "vesSetGet.h"
#include <list>

// -----------------------------------------------------------------pre-defines
class vesActorInternal;
class vesShader;
class vesMapper;
class Appearance;
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
  vesSetGetMacro(Sensor,bool)
  vesSetGetMacro(Widget,vesMultitouchWidget*);
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  bool SetVisible(bool value);
  bool isVisible();
  vesMapper* GetMapper();
  void SetColor(float r, float g, float b, float a);

protected:
  // ...........................................................protected-ivars
  Appearance *_appearance;
  Shape *_shape;
  bool Sensor;
  vesMultitouchWidget* Widget;
  vesMapper *Mapper;
  bool Visible;
protected:
//BTX
  // .......................................................................BTX
  void AddShapeChild(Shape* shape);
private:
  vesActorInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vesActor_h
