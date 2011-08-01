// ============================================================================
/**
 * @file   Matrix4VertexAttribute.cpp
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
#include "vsg/Shader/Matrix4VertexAttribute.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"

namespace vsg{
// ....................................................................internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct Matrix4VertexAttributeInternal
{
  double value; // sample

};
// ....................................................................internal

// -----------------------------------------------------------------------cnstr
Matrix4VertexAttribute::Matrix4VertexAttribute()
{
  _internal = new Matrix4VertexAttributeInternal();
}

// -----------------------------------------------------------------------destr
Matrix4VertexAttribute::~Matrix4VertexAttribute()
{
  delete _internal;
}
}

bool vsg::Matrix4VertexAttribute::accept(vsgVisitor* vsgVisitor)
{
  return vsgVisitor->visitMatrix4VertexAttribute(this);
}
