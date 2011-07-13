/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Shape.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "Shape.h"

// --------------------------------------------------------------------includes
#include <vsgAppearanceNode.h>
#include <vsgGeometryNode.h>
#include "Painter.h"
#include "vesMapper.h"
// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct ShapeInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
Shape::Shape()
{
  this->Internal = new ShapeInternal();
}

// -----------------------------------------------------------------------destr
Shape::~Shape()
{
  delete this->Internal;
}

bool Shape::Read()
{
  std::cout << "Read: Shape" <<std::endl;
  GetAppearance() -> Read();
  if (GetGeometry()) 
  {
    GetGeometry() -> Read();
  }
  return true;
}

void Shape::Render(Painter* render)
{
  render->visitShape(this);
}

void Shape::ComputeBounds()
{
  vesMapper* mapper = (vesMapper*) GetGeometry();
  if(mapper)
  {
  mapper->ComputeBounds();
  SetBBoxCenter(mapper->GetMin(),mapper->GetMax());
  SetBBoxSize(mapper->GetMin(),mapper->GetMax());
  }
}



