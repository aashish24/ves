// ============================================================================
/**
 * @file   vsgTextureNode.h
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
 * This abstract node type is the base type for all node types which specify
 * sources for texture images.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgTextureNode_h
#define __vsgTextureNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Shape/vsgAppearanceChildNode.h"
// -------------------------------------------------------------pre-defines

// -------------------------------------------------------------------class
class vsgTextureNode:public vsgAppearanceChildNode
{
public:
  // ........................................................public-methods
  vsgTextureNode();
  virtual ~vsgTextureNode();

protected:
  // .......................................................protected-ivars

};

#endif // __vsgTextureNode_h
