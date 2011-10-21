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
 * The Matrix4VertexAttribute node defines a set of per-vertex 4×4 matrix attributes.
 *
 * The value field specifies an arbitrary collection of matrix values that will
 * be passed to the shader as per-vertex information. The specific type mapping
 * to the individual shading language data types shall be found in the
 * appropriate language-specific annex (see Table 31.2).
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */

#ifndef __Matrix4VertexAttribute_h
#define __Matrix4VertexAttribute_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgVertexAttributeNode.h"

namespace vsg {

class Matrix4VertexAttributeInternal;

class Matrix4VertexAttribute : public vsgVertexAttributeNode
{
public:
  Matrix4VertexAttribute();
  virtual ~Matrix4VertexAttribute();
  InOutMF(value,MFMatrix4f)
  bool accept(vsgVisitor* vsgVisitor);

private:
  Matrix4VertexAttributeInternal *_internal;
};

}

#endif // __Matrix4VertexAttribute_h
