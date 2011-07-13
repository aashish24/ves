/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesController.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesController -
// .SECTION Description
// vesController

#ifndef __vesController_h
#define __vesController_h
// --------------------------------------------------------------------includes
#include "vtkTransform.h"
#include "vtkShape.h"

// -----------------------------------------------------------------pre-defines
class vesControllerInternal;

// -----------------------------------------------------------------------class
class vesController
{
public:
  // ............................................................public-methods
  vesController();
  ~vesController();
  void Transform(vtkTransform* object);
  void Shape(vtkShape* object);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vesControllerInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vesController_h
