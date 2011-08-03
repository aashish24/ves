// ============================================================================
/**
 * @file   PixelTexture.h
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
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __PixelTexture_h
#define __PixelTexture_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Texture/vsgTexture2DNode.h"

namespace vsg {
  // -------------------------------------------------------------pre-defines
  class PixelTextureInternal;

  // -------------------------------------------------------------------class
  class PixelTexture: public vsgTexture2DNode
  {
  public:
    // ........................................................public-methods
    PixelTexture();
    virtual ~PixelTexture();
    InOutSF(image,SFImage)

  protected:
    // .......................................................protected-ivars

  private:
    // .........................................................private-ivars
    PixelTextureInternal *_internal;
  };
}
#endif // __PixelTexture_h
