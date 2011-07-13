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
#include "vesGMTL.h"
#include "vtkShaderProgram.h"
#include "vtkTransform.h"
#include "vtkShape.h"
#include "vesSetGet.h"
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

  vesMatrix4x4f Eval();
  bool Read();
  void ComputeBounds();
  void Render(vtkPainter* render);
  vesSetGetMacro(Sensor,bool)
  vesSetGetMacro(Widget,vtkMultitouchWidget*);
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  bool SetVisible(bool value);
  bool isVisible();

protected:
  // ...........................................................protected-ivars
  vtkAppearance *Appearance;
  vtkShape *Shape;
  bool Sensor;
  vtkMultitouchWidget* Widget;
  vesMatrix4x4f Matrix;
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
