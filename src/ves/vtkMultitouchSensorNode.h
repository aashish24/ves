/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMultitouchSensorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMultitouchSensorNode -
// .SECTION Description
// vtkMultitouchSensorNode

#ifndef __vtkMultitouchSensorNode_h
#define __vtkMultitouchSensorNode_h
// --------------------------------------------------------------------includes
#include "vtkSensorNode.h"

// -----------------------------------------------------------------pre-defines


// -----------------------------------------------------------------------class
class vtkMultitouchSensorNode : public vtkSensorNode
{
public:
  // ............................................................public-methods
  vtkMultitouchSensorNode();
  ~vtkMultitouchSensorNode();

protected:
  // ...........................................................protected-ivars
  float Translation[2];
  float Scale;
  float Rotation[2];

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX
};

#endif // __vtkMultitouchSensorNode_h
