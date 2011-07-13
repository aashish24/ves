/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShapeNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkShapeNode - A basic viewable object
// .SECTION Description
// vtkShapeNode describes a basic viewable object.

#ifndef __vtkShapeNode_h
#define __vtkShapeNode_h
// --------------------------------------------------------------------includes
#include "vtkChildNode.h"
#include "vtkBoundedObject.h"
#include "vesSetGet.h"

// -----------------------------------------------------------------pre-defines
class vtkShapeNodeInternal;
class vtkAppearanceNode;
class vtkGeometryNode;

// -----------------------------------------------------------------------class
class vtkShapeNode : public vtkChildNode, public vtkBoundedObject
{
public:
  // ............................................................public-methods
  vtkShapeNode();
  ~vtkShapeNode();
  vesSetGetMacro(Appearance,vtkAppearanceNode*)
  vesSetGetMacro(Geometry,vtkGeometryNode*)
protected:
  // ...........................................................protected-ivars
  vtkAppearanceNode * Appearance;
  vtkGeometryNode *Geometry;

//BTX
  // .......................................................................BTX
private:
  vtkShapeNodeInternal *Internal;
  // vtkShapeNode(const vtkShapeNode&); // Not implemented.
  // void operator=(const vtkShapeNode&); // Not implemented.
//ETX
  // .......................................................................ETX
};

#endif // __vtkShapeNode_h

