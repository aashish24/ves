// ============================================================================
/**
 * @file   vsgProgrammableShaderObject.h
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
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgProgrammableShaderObject_h
#define __vsgProgrammableShaderObject_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"

// -------------------------------------------------------------pre-defines

// -------------------------------------------------------------------class
class vsgProgrammableShaderObject
{
public:
  // ........................................................public-methods
  vsgProgrammableShaderObject();
  virtual ~vsgProgrammableShaderObject();

protected:
  // .......................................................protected-ivars
};

#endif // __vsgProgrammableShaderObject_h
