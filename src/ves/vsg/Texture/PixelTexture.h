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
 * ISO-IEC-19775-1.2-X3D-AbstractSpecification: 18.4.6
 *
 * The PixelTexture node defines a 2D image-based texture map as an explicit
 * array of pixel values (image field) and parameters controlling tiling
 * repetition of the texture onto geometry.
 *
 * The repeatS and repeatT fields specify how the texture wraps in the S and T
 * directions. If repeatS is TRUE (the default), the texture map is repeated
 * outside the 0-to-1 texture coordinate range in the S direction so that it
 * fills the shape. If repeatS is FALSE, the texture coordinates are clamped in
 * the S direction to lie within the 0.0 to 1.0 range. The repeatT field is
 * analogous to the repeatS field.
 *
 * See 18.2 Concepts, for a general description of texture maps.
 *
 * See 17 Lighting component for a description of how the texture values
 * interact with the appearance of the geometry. 5.7 SFImage and MFImage
 * describes the specification of an image.
 */

#ifndef __PixelTexture_h
#define __PixelTexture_h

#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Texture/vsgTexture2DNode.h"

namespace vsg {

class PixelTextureInternal;

class PixelTexture: public vsgTexture2DNode
{
public:
  PixelTexture();
  virtual ~PixelTexture();
  InOutSF(image,SFImage)

private:
  PixelTextureInternal *_internal;
};
}
#endif // __PixelTexture_h
