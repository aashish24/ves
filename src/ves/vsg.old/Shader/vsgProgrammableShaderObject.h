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
 * This abstract node type is the base type for all node types that specify
 * arbitrary fields for interfacing with per-object attribute values.
 *
 * A concrete vsgProgrammableShaderObject node instance is used to program
 * behaviour for a shader in a scene. The shader is able to receive and process
 * events that are sent to it. Each event that can be received shall be declared
 * in the shader node using the same field syntax as is used in a prototype
 * definition:
 *
 * inputOnly type name
 *
 * The type can be any of the standard fields. The name shall be an identifier
 * that is unique for this shader node and is used to map the value to the
 * shader program's uniform variable of the same name. If a shader program does
 * not have a matching uniform variable, the field value is ignored.
 *
 * OutputOnly fields are not required to generate output events from a
 * shader. Current hardware shader technology does not support this capability,
 * though future versions may.
 *
 * It is recommended that user-defined field or event names defined in shader
 * nodes follow the naming conventions described in Part 2 of ISO/IEC 19775.
 */

#ifndef __vsgProgrammableShaderObject_h
#define __vsgProgrammableShaderObject_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"

class vsgProgrammableShaderObject
{
public:
  vsgProgrammableShaderObject();
  virtual ~vsgProgrammableShaderObject();
};

#endif // __vsgProgrammableShaderObject_h
