/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesMultitouchSensorNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesMultitouchSensorNode -
// .SECTION Description
// vesMultitouchSensorNode

#ifndef __vesMultitouchSensorNode_h
#define __vesMultitouchSensorNode_h
// --------------------------------------------------------------------includes
#include "vsgSensorNode.h"
#include "vesGMTL.h"
#include "vesSetGet.h"
// -----------------------------------------------------------------pre-defines


// -----------------------------------------------------------------------class
class vesMultitouchSensorNode : public vsgSensorNode
{
public:
  // ............................................................public-methods
  vesMultitouchSensorNode();
  ~vesMultitouchSensorNode();
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

#endif // __vesMultitouchSensorNode_h
