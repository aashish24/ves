// ============================================================================
/**
 * @file   vsgGeometryNode.h
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
 * Base node for all geometry
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgGeometryNode_h
#define __vsgGeometryNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgGeometryNode: public vsgNode
    {
      public:
      // ........................................................public-methods
      vsgGeometryNode();
      virtual ~vsgGeometryNode();

      protected:
      // .......................................................protected-ivars

    };
#endif // __vsgGeometryNode_h
