/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkController.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkController -
// .SECTION Description
// vtkController

#ifndef __vtkController_h
#define __vtkController_h
// --------------------------------------------------------------------includes
#include "vtkTransform.h"
#include "vtkShape.h"

// -----------------------------------------------------------------pre-defines
class vtkControllerInternal;

// -----------------------------------------------------------------------class
class vtkController
{
public:
  // ............................................................public-methods
  vtkController();
  ~vtkController();
  void Transform(vtkTransform* object);
  void Shape(vtkShape* object);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkControllerInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vtkController_h
