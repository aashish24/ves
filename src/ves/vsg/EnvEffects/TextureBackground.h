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
 * From: ISO-IEC-19775-1.2-X3D: 24.4.5
 *
 * The TextureBackground node uses six individual texture nodes to compose the
 * backdrop. Unlike the Background node, which only supports static image
 * formats referenced by URL fields, the contents of the TextureBackground node
 * can be arbitrary texture types, including ImageTexture, PixelTexture,
 * MovieTexture and MultiTexture. The common fields of the TextureBackground
 * node are described in 24.2 Concepts.
 *
 * TextureBackground supports the creation of rich backgrounds with
 * animation. It also allows the world author to attach load sensors (see 9.4.3
 * LoadSensor) to the node's texture fields to receive notification of when
 * elements of the background are loaded.
 *
 * TextureBackground supports a transparency value that allows the scene to
 * overlay other elements in an application. A transparency value of zero
 * specifies that the background is fully opaque obscuring all content in the
 * underlying window. A transparency value of one specifies that the background
 * specified by the TextureBackground node is fully transparent causing the
 * TextureBackground to not be visible so that all underlying content appears as
 * the background. The value of the transparency field is applied to the
 * skyColor and groundColor by first converting the transparency value to an
 * alpha value using the formula:
 *
 * alpha = (1 - transparency)
 *
 * The alpha value is then multiplied against the components of the skyColor and
 * groundColor (including the alpha component, if provided) to obtain the color
 * that is applied to the underlying window content. The transparency value is
 * not applied to the six texture fields. Transparency of these fields can be
 * achieved by using alpha values within their images.
 *
 * For the backTexture, bottomTexture, frontTexture, leftTexture, rightTexture,
 * topTexture fields, browsers shall support any vsgTexture node types supported
 * in the currently supported profile.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */

#ifndef __TextureBackground_h
#define __TextureBackground_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/EnvEffects/vsgBackgroundNode.h"

namespace vsg {
class TextureBackgroundInternal;

class TextureBackground: public vsgBackgroundNode
{
public:
  TextureBackground();
  virtual ~TextureBackground();
  InOutSF(backTexture,SFNode)
  InOutSF(bottomTexture,SFNode)
  InOutSF(frontTexture,SFNode)
  InOutSF(leftTexture,SFNode)
  InOutSF(rightTexture,SFNode)
  InOutSF(topTexture, SFNode)
  private:
    TextureBackgroundInternal *_internal;
};
}
#endif // __TextureBackground_h
