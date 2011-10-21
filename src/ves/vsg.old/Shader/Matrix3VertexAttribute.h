/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

/**
 * @section DESCRIPTION
 *
 * The Matrix3VertexAttribute node defines a set of per-vertex 3×3 matrix attributes.

The value field specifies an arbitrary collection of matrix values that will be passed to the shader as per-vertex information. The specific type mapping to the individual shading language data types shall be found in the appropriate language-specific annex (see Table 31.2).
 */

#ifndef __Matrix3VertexAttribute_h
#define __Matrix3VertexAttribute_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgVertexAttributeNode.h"

namespace vsg {

class Matrix3VertexAttributeInternal;

class Matrix3VertexAttribute: public vsgVertexAttributeNode
{
public:
  Matrix3VertexAttribute();
  virtual ~Matrix3VertexAttribute();
  InOutMF(value,MFMatrix3f)
  bool accept(vsgVisitor* vsgVisitor);

private:
  Matrix3VertexAttributeInternal *_internal;
};

}

#endif // __Matrix3VertexAttribute_h
