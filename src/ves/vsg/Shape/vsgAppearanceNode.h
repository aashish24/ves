// ============================================================================
/**
 * @file   vsgAppearanceNode.h
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
 * This is the base node type for all Appearance nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgAppearanceNode_h
#define __vsgAppearanceNode_h
// --------------------------------------------------------------------includes
# include "vsg//Utility/vsgMacro.h"
# include "vsg/Utility/vsgTypes.h"
# include "vsg/Core/vsgNode.h"

    // -------------------------------------------------------------pre-defines

    // -------------------------------------------------------------------class
    class vsgAppearanceNode: public vsgNode
    {
      public:
      // ........................................................public-methods
      vsgAppearanceNode();
      virtual ~vsgAppearanceNode();

      protected:
      // .......................................................protected-ivars

    };

#endif // __vsgAppearanceNode_h
