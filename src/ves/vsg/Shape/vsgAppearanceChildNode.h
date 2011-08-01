// ============================================================================
/**
 * @file   vsgAppearanceChildNode.h
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
 * This is the base node type for the child nodes of the X3DAppearanceNode type.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgAppearanceChildNode_h
#define __vsgAppearanceChildNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgAppearanceChildNode: public vsgNode
    {
      public:
      // ........................................................public-methods
      vsgAppearanceChildNode();
      virtual ~vsgAppearanceChildNode();

      protected:
      // .......................................................protected-ivars

    };
#endif // __vsgAppearanceChildNode_h
