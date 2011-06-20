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

#include <list>

// -----------------------------------------------------------------pre-defines
class vtkActorInternal;
class vtkShader;
class vtkMapper;
class vtkAppearance;

// -----------------------------------------------------------------------class
class vtkActor : public vtkTransform
{
public:
  // ............................................................public-methods
  vtkActor(vtkShader* shader,vtkMapper* mapper);
  ~vtkActor();

  vtkMatrix4x4f Eval();
  bool Read();
  void Render(vtkPainter* render);

protected:
  // ...........................................................protected-ivars
  vtkAppearance *Appearance;
  vtkShape *Shape;

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
