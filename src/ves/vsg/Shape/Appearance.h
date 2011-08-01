// ============================================================================
/**
 * @file   Appearance.h
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
 * The Appearance node specifies the visual properties of geometry. The value
 * for each of the fields in this node may be NULL. However, if the field is
 * non-NULL, it shall contain one node of the appropriate type.
 *
 * The material field, if specified, shall contain a Material node. If the
 * material field is NULL or unspecified, lighting is off (all lights are
 * ignored during rendering of the object that references this Appearance) and
 * the unlit object colour is (1, 1, 1). Details of the X3D lighting model are
 * in 17 Lighting component.
 *
 * The texture field, if specified, shall contain one of the various types of
 * texture nodes (see 18 Texturing component). If the texture node is NULL or
 * the texture field is unspecified, the object that references this Appearance
 * is not textured.
 *
 * The textureTransform field, if specified, shall contain a TextureTransform
 * node as defined in TextureTransform. If the textureTransform is NULL or
 * unspecified, the textureTransform field has no effect.
 *
 * The lineProperties field, if specified, shall contain a LineProperties node
 * as specified in LineProperties. If lineProperties is NULL or unspecified, the
 * lineProperties field has no effect.
 *
 * The fillProperties field, if specified, shall contain a FillProperties node
 * as specified in FillProperties. If fillProperties is NULL or unspecified, the
 * fillProperties field has no effect.
 *
 * The shaders field contains a listing, in order of preference, of nodes that
 * describe programmable shaders that replace the fixed rendering requirements
 * of this part of ISO/IEC 19775 with user-provided functionality. If the field
 * is not empty, one shader node is selected and the fixed rendering
 * requirements defined by this specification are ignored. The field shall
 * contain one of the various types of shader nodes as specified in 31
 * Programmable shaders component.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Appearance_h
#define __Appearance_h
// --------------------------------------------------------------------includes
# include "vsg/Utility/vsgMacro.h"
# include "vsg/Utility/vsgTypes.h"
#include "vsg/Shape/vsgAppearanceNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class AppearanceInternal;

    // -------------------------------------------------------------------class
    class Appearance: public vsgAppearanceNode
    {
      public:
      // ........................................................public-methods
      Appearance();
      virtual ~Appearance();
      bool Read();              // This will be deleted in the future
      void Render(Painter* render); // This will be deleted in the future

      // InOutSF(fillProperties,SFNode)
      // InOutSF(lineProperties,SFNode)
      // InOutSF(maternal,SFNode)
      InOutMF(shaders,MFNode)
      // InOutSF(texture,SFNode)
      // InOutSF(textureTransform,SFNode)
      bool accept(vsgVisitor* vsgVisitor);
      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      AppearanceInternal *_internal;
    };
}
#endif // __Appearance_h
