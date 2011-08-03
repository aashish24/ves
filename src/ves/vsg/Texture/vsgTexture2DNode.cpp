// ============================================================================
/**
 * @file   vsgTexture2DNode.cpp
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
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
# include "vsg/Texture/vsgTexture2DNode.h"
// --------------------------------------------------------------------includes

// -------------------------------------------------------------------macro

// -------------------------------------------------------------------cnstr
vsgTexture2DNode::vsgTexture2DNode()
{
  _repeatS = true;
  _repeatT = true;
  _textureProperties = NULL;
}

vsgTexture2DNode::vsgTexture2DNode(bool repeatS,
                                   bool repeatT,
                                   SFNode textureProperties)
{
  _repeatS = repeatS;
  _repeatT = repeatT;
  _textureProperties = textureProperties;
}

// -------------------------------------------------------------------destr
vsgTexture2DNode::~vsgTexture2DNode()
{
}
// ------------------------------------------------------------------public

// -----------------------------------------------------------------private

