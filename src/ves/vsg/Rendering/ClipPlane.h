// ============================================================================
/**
 * @file   ClipPlane.h
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
 * The ClipPlane node specifies a single plane equation that will be used to
 * clip the geometry. The plane field specifies a four-component plane equation
 * that describes the inside and outside half space. The first three components
 * are a normalized vector describing the direction of the plane's normal
 * direction.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __ClipPlane_h
#define __ClipPlane_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgChildNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class ClipPlaneInternal;

    // -------------------------------------------------------------------class
    class ClipPlane: public vsgChildNode
    {
      public:
      // ........................................................public-methods
      ClipPlane();
      virtual ~ClipPlane();
      InOutSF(enabled,SFBool)
      InOutSF(plane,SFVec4f)
      bool accept(vsgVisitor *vsgVisitor);

      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      ClipPlaneInternal *_internal;
    };
}
#endif // __ClipPlane_h
