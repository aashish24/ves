// ============================================================================
/**
 * @file   vsgColorNode.h
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
 *
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgColorNode_h
#define __vsgColorNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgGeometricPropertyNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgColorNode : public vsgGeometricPropertyNode
    {
      public:
      // ........................................................public-methods
      vsgColorNode();
      virtual ~vsgColorNode();

      protected:
      // .......................................................protected-ivars

    };
#endif // __vsgColorNode_h
