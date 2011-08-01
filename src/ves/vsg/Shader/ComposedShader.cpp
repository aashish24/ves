// ============================================================================
/**
 * @file   ComposedShader.cpp
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
#include "vsg/Shader/ComposedShader.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"

namespace vsg {
// ....................................................................internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct ComposedShaderInternal
{
  double value; // sample

};
// ....................................................................internal

// -----------------------------------------------------------------------cnstr
ComposedShader::ComposedShader()
{
  _internal = new ComposedShaderInternal();
}

// -----------------------------------------------------------------------destr
ComposedShader::~ComposedShader()
{
  delete _internal;
}
}

bool vsg::ComposedShader::accept(vsgVisitor *vsgVisitor)
{
  return vsgVisitor->visitComposedShader(this);
}
