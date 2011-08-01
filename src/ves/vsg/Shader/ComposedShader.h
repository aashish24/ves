// ============================================================================
/**
 * @file   ComposedShader.h
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
 *
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __ComposedShader_h
#define __ComposedShader_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shader/vsgShaderNode.h"
#include "vsg/Shader/vsgProgrammableShaderObject.h"
namespace vsg{
// -----------------------------------------------------------------pre-defines
class ComposedShaderInternal;

// -----------------------------------------------------------------------class
class ComposedShader: public vsgShaderNode, public vsgProgrammableShaderObject
{
public:
  // ............................................................public-methods
  ComposedShader();
  virtual ~ComposedShader();
  InOutMF(parts,MFNode)
  bool accept(vsgVisitor *vsgVisitor);

protected:
  // ...........................................................protected-ivars

  private:
  // .............................................................private-ivars
  ComposedShaderInternal *_internal;
};
}
#endif // __ComposedShader_h
