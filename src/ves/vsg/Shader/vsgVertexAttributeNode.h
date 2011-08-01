// ============================================================================
/**
 * @file   vsgVertexAttributeNode.h
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
 * per-vertex attribute information to the shader.
 *
 * The name field describes a name that is mapped to the shading
 * language-specific name for describing per-vertex data.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgVertexAttributeNode_h
#define __vsgVertexAttributeNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Render/vsgGeometricPropertyNode.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgVertexAttributeNode: public vsgGeometricPropertyNode
{
public:
  // ............................................................public-methods
  vsgVertexAttributeNode();
  virtual ~vsgVertexAttributeNode();

protected:
  // ...........................................................protected-ivars
  SFString _name;
};
#endif // __vsgVertexAttributeNode_h
