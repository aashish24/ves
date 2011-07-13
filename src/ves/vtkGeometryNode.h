/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGeometryNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkGeometryNode - describes geometry
// .SECTION Description
// vtkGeometryNode describes geometry

#ifndef __vtkGeometryNode_h
#define __vtkGeometryNode_h
// --------------------------------------------------------------------includes
#include "vtkNode.h"
#include "vesGMTL.h"
// -----------------------------------------------------------------pre-defines
class vtkGeometryNodeInternal;

// -----------------------------------------------------------------------class
class vtkGeometryNode : public vtkNode
{
public:
  // ............................................................public-methods
  vtkGeometryNode();
  ~vtkGeometryNode();
  
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkGeometryNodeInternal *Internal;
  // vtkGeometryNode(const vtkGeometryNode&); // Not implemented.
  // void operator=(const vtkGeometryNode&); // Not implemented.
//ETX
  // .......................................................................ETX


};

#endif // __vtkGeometryNode_h
