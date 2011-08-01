/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgGeometryNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgGeometryNode - describes geometry
// .SECTION Description
// vsgGeometryNode describes geometry

#ifndef __vsgGeometryNode_h
#define __vsgGeometryNode_h
// --------------------------------------------------------------------includes
#include "vsgNode.h"
#include "vesGMTL.h"
// -----------------------------------------------------------------pre-defines
class vsgGeometryNodeInternal;

// -----------------------------------------------------------------------class
class vsgGeometryNode : public vsgNode
{
public:
  // ............................................................public-methods
  vsgGeometryNode();
  ~vsgGeometryNode();
  
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vsgGeometryNodeInternal *Internal;
  // vsgGeometryNode(const vsgGeometryNode&); // Not implemented.
  // void operator=(const vsgGeometryNode&); // Not implemented.
//ETX
  // .......................................................................ETX


};

#endif // __vsgGeometryNode_h
