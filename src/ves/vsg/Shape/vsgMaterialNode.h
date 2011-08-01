// ============================================================================
/**
 * @file   vsgMaterialNode.h
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
 * This is the base node type for all Material nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgMaterialNode_h
#define __vsgMaterialNode_h
// --------------------------------------------------------------------includes
# include "vsg/Utility/vsgMacro.h"
# include "vsg/Utility/vsgTypes.h"
# include "vsg/Shape/vsgAppearanceChildNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgMaterialNode: public vsgAppearanceChildNode
    {
      public:
      // ........................................................public-methods
      vsgMaterialNode();
      virtual ~vsgMaterialNode();

      protected:
      // .......................................................protected-ivars

    };

#endif // __vsgMaterialNode_h
