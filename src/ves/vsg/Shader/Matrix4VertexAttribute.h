// ============================================================================
/**
 * @file   Matrix4VertexAttribute.h
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
 * @section DESCRIPTION
 *
 * The Matrix4VertexAttribute node defines a set of per-vertex 4×4 matrix attributes.
 *
 * The value field specifies an arbitrary collection of matrix values that will
 * be passed to the shader as per-vertex information. The specific type mapping
 * to the individual shading language data types shall be found in the
 * appropriate language-specific annex (see Table 31.2).
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Matrix4VertexAttribute_h
#define __Matrix4VertexAttribute_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgVertexAttributeNode.h"

namespace vsg {
// -----------------------------------------------------------------pre-defines
class Matrix4VertexAttributeInternal;

// -----------------------------------------------------------------------class
class Matrix4VertexAttribute : public vsgVertexAttributeNode
{
public:
  // ............................................................public-methods
  Matrix4VertexAttribute();
  virtual ~Matrix4VertexAttribute();
  InOutMF(value,MFMatrix4f)
  bool accept(vsgVisitor* vsgVisitor);
  protected:
  // ...........................................................protected-ivars

  private:
  // .............................................................private-ivars
  Matrix4VertexAttributeInternal *_internal;
};
}
#endif // __Matrix4VertexAttribute_h
