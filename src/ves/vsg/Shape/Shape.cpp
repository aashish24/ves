// ============================================================================
/**
 * @file   Shape.cpp
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#include "vsg/Shape/Shape.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"
#include "Painter.h"

namespace vsg {
  // -------------------------------------------------------------------macro

  // ................................................................internal
  // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
  // be put in the class declaration. For all newly defined pointers make sure
  // to update constructor and destructor methods.
  struct ShapeInternal
  {
    double value; // sample

  };
  // ................................................................internal

  // -------------------------------------------------------------------cnstr
  Shape::Shape()
  {
    _internal = new ShapeInternal();
  }

  // -------------------------------------------------------------------destr
  Shape::~Shape()
  {
    delete _internal;
  }

  // ------------------------------------------------------------------public
  // -----------------------------------------------------------------private
}

bool vsg::Shape::accept(vsgVisitor* vsgVisitor)
{
  return vsgVisitor->visitShape(this);
}
bool vsg::Shape::Read()
{
  //std::cout << "Read: Shape" <<std::endl;
  get_appearance() -> Read();
  if (get_geometry())
    {
      get_geometry() -> Read();
    }
  return true;
}

void vsg::Shape::Render(Painter* render)
{
  render->visitShape(this);
}

void vsg::Shape::ComputeBounds()
{
  vesMapper* mapper = (vesMapper*) get_geometry();
  if(mapper)
    {
      mapper->ComputeBounds();
      SetBBoxCenter(mapper->GetMin(),mapper->GetMax());
      SetBBoxSize(mapper->GetMin(),mapper->GetMax());
    }
}



