// ============================================================================
/**
 * @file   Matrix3VertexAttribute.cpp
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
#include "vsg/Shader/Matrix3VertexAttribute.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"
namespace vsg {
    // -------------------------------------------------------------------macro

    // ................................................................internal
    // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
    // be put in the class declaration. For all newly defined pointers make sure
    // to update constructor and destructor methods.
    struct Matrix3VertexAttributeInternal
    {
      double value; // sample

    };
    // ................................................................internal

    // -------------------------------------------------------------------cnstr
    Matrix3VertexAttribute::Matrix3VertexAttribute()
       {
         _internal = new Matrix3VertexAttributeInternal();
       }

    // -------------------------------------------------------------------destr
    Matrix3VertexAttribute::~Matrix3VertexAttribute()
       {
         delete _internal;
       }

    // ------------------------------------------------------------------public
    // -----------------------------------------------------------------private
}

bool vsg::Matrix3VertexAttribute::accept(vsgVisitor* visitor)
{
  return visitor->visitMatrix3VertexAttribute(this);
}
