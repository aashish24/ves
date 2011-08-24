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
 * per-vertex attribute information to the shader.
 *
 * The name field describes a name that is mapped to the shading
 * language-specific name for describing per-vertex data.
 */

#ifndef __vsgVertexAttributeNode_h
#define __vsgVertexAttributeNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Rendering/vsgGeometricPropertyNode.h"

class vsgVertexAttributeNode: public vsgGeometricPropertyNode
{
public:
  vsgVertexAttributeNode();
  virtual ~vsgVertexAttributeNode();

protected:
  SFString _name;
};

#endif // __vsgVertexAttributeNode_h
