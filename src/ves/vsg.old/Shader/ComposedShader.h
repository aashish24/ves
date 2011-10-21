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
 * The ComposedShader node defines a shader where the individual source files
 * are not individually programmable. All access to the shading capabilities is
 * defined through a single interface that applies to all parts.
 *
 * EXAMPLE  OpenGL Shading Language (GLSL)
 *
 * The isValid field adds an additional semantic indicating whether the current
 * shader parts can be linked together to form a complete valid shader program.
 *
 * The activate field forces the shader to activate the contained objects. The
 * conditions under which a activate may be required are described in I.5 Event
 * model.
 */

#ifndef __ComposedShader_h
#define __ComposedShader_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgShaderNode.h"
#include "vsg/Shader/vsgProgrammableShaderObject.h"

namespace vsg{

class ComposedShaderInternal;

class ComposedShader: public vsgShaderNode, public vsgProgrammableShaderObject
{
public:
  ComposedShader();
  virtual ~ComposedShader();
  InOutMF(parts,MFNode)
  bool accept(vsgVisitor *vsgVisitor);

private:
  ComposedShaderInternal *_internal;
};

}

#endif // __ComposedShader_h
