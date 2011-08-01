/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTextureCoordinateNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkTextureCoordinateNode - abstract base type
// .SECTION Description
// vtkTextureCoordinateNode is the base type for all node types which specify
// texture coordinates

#ifndef __vtkTextureCoordinateNode_h
#define __vtkTextureCoordinateNode_h
// --------------------------------------------------------------------includes
#include "vsgGeometricPropertyNode.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vtkTextureCoordinateNode : public vsgGeometricPropertyNode
{
public:
  // ............................................................public-methods
  vtkTextureCoordinateNode();
  ~vtkTextureCoordinateNode();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX
private:
//ETX
  // .......................................................................ETX


};

#endif // __vtkTextureCoordinateNode_h
