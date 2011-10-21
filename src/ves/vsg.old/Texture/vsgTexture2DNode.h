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
 * This abstract node type is the base type for all node types which specify 2D
 * sources for texture images.
 */

#ifndef __vsgTexture2DNode_h
#define __vsgTexture2DNode_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Texture/vsgTextureNode.h"

class vsgTexture2DNode: public vsgTextureNode
{
public:
  vsgTexture2DNode();
  vsgTexture2DNode(bool repeatS, bool repeatT , SFNode textureProperties);
  virtual ~vsgTexture2DNode();
protected:
  bool _repeatS;
  bool _repeatT;
  SFNode _textureProperties;
};

#endif // __vsgTexture2dNode_h
