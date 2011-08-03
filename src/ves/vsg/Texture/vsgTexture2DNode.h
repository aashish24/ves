// ============================================================================
/**
 * @file   vsgTexture2DNode.h
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
 * This abstract node type is the base type for all node types which specify 2D
 * sources for texture images.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgTexture2DNode_h
#define __vsgTexture2DNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Texture/vsgTextureNode.h"
// -------------------------------------------------------------pre-defines

// -------------------------------------------------------------------class
class vsgTexture2DNode: public vsgTextureNode
{
public:
  // ........................................................public-methods
  vsgTexture2DNode();
  vsgTexture2DNode(bool repeatS, bool repeatT , SFNode textureProperties);
  virtual ~vsgTexture2DNode();
protected:
  // .......................................................protected-ivars
  bool _repeatS;
  bool _repeatT;
  SFNode _textureProperties;
};
#endif // __vsgTexture2dNode_h
