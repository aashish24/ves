/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShape.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkShape - Concrete Shape Class
// .SECTION Description
// vtkShape This describes the concrete shape class

#ifndef __vtkShape_h
#define __vtkShape_h
// --------------------------------------------------------------------includes
#include "vtkShapeNode.h"

// -----------------------------------------------------------------pre-defines
class vtkShapeInternal;

// -----------------------------------------------------------------------class
class vtkShape : public vtkShapeNode
{
public:
  // ............................................................public-methods
  vtkShape();
  ~vtkShape();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkShapeInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vtkShape_h
