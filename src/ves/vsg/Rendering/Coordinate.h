// ============================================================================
/**
 * @file   Coordinate.h
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
 * This node defines a set of 3D coordinates to be used in the coord field of
 * vertex-based geometry nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __Coordinate_h
#define __Coordinate_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgCoordinateNode.h"

namespace vsg {
    // -------------------------------------------------------------pre-defines
    class CoordinateInternal;

    // -------------------------------------------------------------------class
    class Coordinate: public vsgCoordinateNode
    {
      public:
      // ........................................................public-methods
      Coordinate();
      virtual ~Coordinate();
      //InOutMF(point, MFVec3f)
      bool accept(vsgVisitor *vsgVisitor);
      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      CoordinateInternal *_internal;
    };
}
#endif // __Coordinate_h
