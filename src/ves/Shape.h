/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Shape.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Shape - Concrete Shape Class
// .SECTION Description
// Shape This describes the concrete shape class

#ifndef __Shape_h
#define __Shape_h
// --------------------------------------------------------------------includes
#include "vsgShapeNode.h"

// -----------------------------------------------------------------pre-defines
class ShapeInternal;

// -----------------------------------------------------------------------class
class Shape : public vsgShapeNode
{
public:
  // ............................................................public-methods
  Shape();
  ~Shape();
  bool Read();
  void Render(Painter* render);
  void ComputeBounds();
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
  
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  ShapeInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __Shape_h
