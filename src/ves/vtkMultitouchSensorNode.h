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
#include "vesGMTL.h"
// -----------------------------------------------------------------pre-defines


// -----------------------------------------------------------------------class
class vtkMultitouchSensorNode : public vtkSensorNode
{
public:
  // ............................................................public-methods
  vtkMultitouchSensorNode();
  ~vtkMultitouchSensorNode();
  vesSetGetMacro(Translation, vesVector3f)
  vesSetGetMacro(Scale, vesVector3f)
  vesSetGetMacro(Rotation,vesVector4f)
protected:
  // ...........................................................protected-ivars
  vesVector3f Translation;
  vesVector3f Scale;
  vesVector4f Rotation;

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX
};

#endif // __vtkMultitouchSensorNode_h
