/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkActor.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkActor - Defines a entity in the scene
// .SECTION Description
// vtkActor

#ifndef __vtkActor_h
#define __vtkActor_h
// --------------------------------------------------------------------includes
#include "vtkGMTL.h"
#include "vtkShaderProgram.h"
#include "vtkTransform.h"
#include "vtkShape.h"
#include "vtkSetGet.h"
#include <list>

// -----------------------------------------------------------------pre-defines
class vtkActorInternal;
class vtkShader;
class vtkMapper;
class vtkAppearance;
class vtkMultitouchWidget;

// -----------------------------------------------------------------------class
class vtkActor : public vtkTransform
{
public:
  // ............................................................public-methods
  vtkActor(vtkShader* shader,vtkMapper* mapper,vtkMultitouchWidget *widget=0);
  ~vtkActor();

  vtkMatrix4x4f Eval();
  bool Read();
  void ComputeBounds();
  void Render(vtkPainter* render);
  vtkSetGetMacro(Sensor,bool)
  vtkSetGetMacro(Widget,vtkMultitouchWidget*);
  vtkGetMacro(Min, vtkVector3f)
  vtkGetMacro(Max, vtkVector3f)
  bool SetVisible(bool value);
  bool isVisible();

protected:
  // ...........................................................protected-ivars
  vtkAppearance *Appearance;
  vtkShape *Shape;
  bool Sensor;
  vtkMultitouchWidget* Widget;
  vtkMatrix4x4f Matrix;
  vtkMapper *Mapper;
  bool Visible;
protected:
//BTX
  // .......................................................................BTX
  void AddShapeChild(vtkShape* shape);
private:
  vtkActorInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vtkActor_h
