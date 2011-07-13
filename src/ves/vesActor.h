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
#include "vtkTransform.h"
#include "vtkShape.h"
#include "vesSetGet.h"
#include <list>

// -----------------------------------------------------------------pre-defines
class vesActorInternal;
class vesShader;
class vesMapper;
class vtkAppearance;
class vesMultitouchWidget;

// -----------------------------------------------------------------------class
class vesActor : public vtkTransform
{
public:
  // ............................................................public-methods
  vesActor(vesShader* shader,vesMapper* mapper,vesMultitouchWidget *widget=0);
  ~vesActor();

  vesMatrix4x4f Eval();
  bool Read();
  void ComputeBounds();
  void Render(vtkPainter* render);
  vesSetGetMacro(Sensor,bool)
  vesSetGetMacro(Widget,vesMultitouchWidget*);
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  bool SetVisible(bool value);
  bool isVisible();

protected:
  // ...........................................................protected-ivars
  vtkAppearance *Appearance;
  vtkShape *Shape;
  bool Sensor;
  vesMultitouchWidget* Widget;
  vesMatrix4x4f Matrix;
  vesMapper *Mapper;
  bool Visible;
protected:
//BTX
  // .......................................................................BTX
  void AddShapeChild(vtkShape* shape);
private:
  vesActorInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vesActor_h
