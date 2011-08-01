// ============================================================================
/**
 * @file   Color.h
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
 * This node defines a set of RGB colours to be used in the fields of another
 * node.
 *
 * Color nodes are only used to specify multiple colours for a single geometric
 * shape, such as colours for the faces or vertices of an IndexedFaceSet. A
 * Material node is used to specify the overall material parameters of lit
 * geometry. If both a Material node and a Color node are specified for a
 * geometric shape, the colours shall replace the diffuse component of the
 * material.
 *
 * RGB or RGBA textures take precedence over colours; specifying both an RGB or
 * RGBA texture and a Color node for geometric shape will result in the Color
 * node being ignored. Details on lighting equations can be found in Lighting
 * model.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Color_h
#define __Color_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgColorNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class ColorInternal;

    // -------------------------------------------------------------------class
    class Color: public vsgColorNode
    {
      public:
      // ........................................................public-methods
      Color();
      virtual ~Color();
      //InOutMF(color,MFColor)
      bool accept(vsgVisitor *vsgVisitor);
      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      ColorInternal *_internal;
    };
}
#endif // __Color_h
