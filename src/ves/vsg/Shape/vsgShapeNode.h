/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgShapeNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgShapeNode - A basic viewable object
// .SECTION Description
// vsgShapeNode describes a basic viewable object.

#ifndef __vsgShapeNode_h
#define __vsgShapeNode_h
// --------------------------------------------------------------------includes
#include "vsgChildNode.h"
#include "vsgBoundedObject.h"
#include "vesSetGet.h"

// -----------------------------------------------------------------pre-defines
class vsgShapeNodeInternal;
class vsgAppearanceNode;
class vsgGeometryNode;

// -----------------------------------------------------------------------class
class vsgShapeNode : public vsgChildNode, public vsgBoundedObject
{
public:
  // ............................................................public-methods
  vsgShapeNode();
  ~vsgShapeNode();
  vesSetGetMacro(Appearance,vsgAppearanceNode*)
  vesSetGetMacro(Geometry,vsgGeometryNode*)
protected:
  // ...........................................................protected-ivars
  vsgAppearanceNode * Appearance;
  vsgGeometryNode *Geometry;

//BTX
  // .......................................................................BTX
private:
  vsgShapeNodeInternal *Internal;
  // vsgShapeNode(const vsgShapeNode&); // Not implemented.
  // void operator=(const vsgShapeNode&); // Not implemented.
//ETX
  // .......................................................................ETX
};

#endif // __ShapeNode_h

