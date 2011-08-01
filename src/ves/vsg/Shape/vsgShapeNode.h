// ============================================================================
/**
 * @file   vsgShapeNode.h
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
 * This is the base node type for all Shape nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgShapeNode_h
#define __vsgShapeNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgChildNode.h"
#include "vsg/Grouping/vsgBoundedObject.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgShapeNode: public vsgChildNode, public vsgBoundedObject
    {
      public:
      // ........................................................public-methods
      vsgShapeNode();
      virtual ~vsgShapeNode();
      InOutSF(appearance,SFNode)
      InOutSF(geometry,SFNode)
      protected:
      // .......................................................protected-ivars

    };
#endif // __vsgShapeNode_h
