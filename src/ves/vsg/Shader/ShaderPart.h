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

#ifndef __ShaderPart_h
#define __ShaderPart_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgNode.h"

namespace vsg {

class ShaderPartInternal;

class ShaderPart: public vsgNode // , public vsgURLObject
{
public:
  ShaderPart();
  virtual ~ShaderPart();
  InOutMF(url,MFString)
  bool accept(vsgVisitor* vsgVisitor);
protected:
  SFString _type;

private:
  ShaderPartInternal *_internal;
};

}

#endif // __ShaderPart_h
