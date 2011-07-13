/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgSensorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgSensorNode -
// .SECTION Description
// vsgSensorNode

#ifndef __vsgSensorNode_h
#define __vsgSensorNode_h
// --------------------------------------------------------------------includes
#include "vsgChildNode.h"
#include "vesSetGet.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgSensorNode : public vsgChildNode
{
public:
  // ............................................................public-methods
  vsgSensorNode();
  ~vsgSensorNode();
  bool IsActive();
  vesSetGetMacro(Enabled,bool)
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

#endif // __vsgSensorNode_h
