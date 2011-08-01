// ============================================================================
/**
 * @file   ColorRGBA.h
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
 * This node defines a set of RGBA colours to be used in the fields of another node.
 *
 * RGBA color nodes are only used to specify multiple colours with alpha for a
 * single geometric shape, such as colours for the faces or vertices of an
 * IndexedFaceSet. A Material node is used to specify the overall material
 * parameters of lit geometry. If both a Material node and a ColorRGBA node are
 * specified for a geometric shape, the colours shall replace the diffuse and
 * transparency components of the material.
 *
 * RGB or RGBA textures take precedence over colours; specifying both an RGB or
 * RGBA texture and a ColorRGBA node for geometric shape will result in the
 * ColorRGBA node being ignored. Details on lighting equations can be found in
 * Lighting model.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __ColorRGBA_h
#define __ColorRGBA_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgColorNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class ColorRGBAInternal;

    // -------------------------------------------------------------------class
    class ColorRGBA: public vsgColorNode
    {
      public:
      // ........................................................public-methods
      ColorRGBA();
      virtual ~ColorRGBA();
      //InOutMF(color, MFColorRGBA)
      bool accept(vsgVisitor *vsgVisitor);
      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      ColorRGBAInternal *_internal;
    };
}
#endif // __ColorRGBA_h
