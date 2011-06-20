/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSensorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSensorNode -
// .SECTION Description
// vtkSensorNode

#ifndef __vtkSensorNode_h
#define __vtkSensorNode_h
// --------------------------------------------------------------------includes
#include "vtkChildNode.h"
#include "vtkSetGet.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vtkSensorNode : public vtkChildNode
{
public:
  // ............................................................public-methods
  vtkSensorNode();
  ~vtkSensorNode();
  bool IsActive();
  vtkSetGetMacro(Enabled,bool)
protected:

  // ...........................................................protected-ivars
  bool Enabled;
protected:
//BTX
  // .......................................................................BTX

private:
//ETX
  // .......................................................................ETX


};

#endif // __vtkSensorNode_h
