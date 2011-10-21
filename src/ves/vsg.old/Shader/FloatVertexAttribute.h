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
 * The FloatVertexAttribute node defines a set of per-vertex single-precision floating point attributes.

The numComponents field specifies how many consecutive floating point values should be grouped together per vertex. The length of the value field shall be a multiple of numComponents.

The value field specifies an arbitrary collection of floating point values that will be passed to the shader as per-vertex information. The specific type mapping to the individual shading language data types is in the appropriate language-specific annex (see Table 31.2).
 */

#ifndef __FloatVertexAttribute_h
#define __FloatVertexAttribute_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgVertexAttributeNode.h"

namespace vsg {

class FloatVertexAttributeInternal;

class FloatVertexAttribute: public vsgVertexAttributeNode
{
public:
  FloatVertexAttribute();
  virtual ~FloatVertexAttribute();
  InOutMF(value,MFFloat)
  bool accept(vsgVisitor *vsgVisitor);
protected:
  SFInt32 _numComponents;
private:
  FloatVertexAttributeInternal *_internal;
};

}

#endif // __FloatVertexAttribute_h
