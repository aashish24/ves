// ============================================================================
/**
 * @file   vsgGeometricPropertyNode.h
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
 * Abstract base type for all Geometric Property Node types
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgGeometricPropertyNode_h
#define __vsgGeometricPropertyNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgNode.h"


    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgGeometricPropertyNode : public vsgNode
    {
      public:
      // ........................................................public-methods
      vsgGeometricPropertyNode();
      virtual ~vsgGeometricPropertyNode();
      InOutSF(data,float)
      InOutSF(size,int)
      InOutSF(stride,int)
      protected:
      // .......................................................protected-ivars

    };
#endif // __vsgGeometricPropertyNode_h
