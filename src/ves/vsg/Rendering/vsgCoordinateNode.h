// ============================================================================
/**
 * @file   vsgCoordinateNode.h
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
 * Abstract Base node for all coordinate node types
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgCoordinateNode_h
#define __vsgCoordinateNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgGeometricPropertyNode.h"

    // -------------------------------------------------------------pre-defines
    class vsgCoordinateNodeInternal;

    // -------------------------------------------------------------------class
    class vsgCoordinateNode: public vsgGeometricPropertyNode
    {
      public:
      // ........................................................public-methods
      vsgCoordinateNode();
      virtual ~vsgCoordinateNode();

      protected:
      // .......................................................protected-ivars

      private:
      // .........................................................private-ivars
      vsgCoordinateNodeInternal *_internal;
    };

#endif // __vsgCoordinateNode_h
