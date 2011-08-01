// ============================================================================
/**
 * @file   ShaderPart.h
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
 * The ShaderPart node defines the source for a single object to be used by a
 * ComposedShader node. The source is not required to be a complete shader for
 * all of the vertex/fragment processing.
 *
 * The type field indicates whether this object shall be compiled as a vertex
 * shader, fragment shader, or other future-defined shader type.
 *
 * The shader source is read from the URL specified by the url field. When the
 * url field contains no values ([]), this object instance is ignored. The url
 * field is defined in 9.2.1 URLs. Shader source files shall be plain text
 * encoded as specified for MIME type text/plain and interpreted according to
 * the type field.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __ShaderPart_h
#define __ShaderPart_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgNode.h"

namespace vsg {

// -----------------------------------------------------------------pre-defines
class ShaderPartInternal;

// -----------------------------------------------------------------------class
class ShaderPart: public vsgNode // , public vsgURLObject
{
public:
  // ............................................................public-methods
  ShaderPart();
  virtual ~ShaderPart();
  InOutMF(url,MFString)
  bool accept(vsgVisitor* vsgVisitor);
  protected:
  // ...........................................................protected-ivars
  SFString _type;

private:
  // .............................................................private-ivars
  ShaderPartInternal *_internal;
};
}
#endif // __ShaderPart_h
