// ============================================================================
/**
 * @file   vsgChildNode.h
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
 * Abstract base class
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgChildNode_h
#define __vsgChildNode_h
// --------------------------------------------------------------------includes
# include "vsg/Utility/vsgMacro.h"
# include "vsg/Utility/vsgTypes.h"
# include "vsg/Core/vsgNode.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgChildNode: public vsgNode
{
public:
  // ............................................................public-methods
  vsgChildNode();
  ~vsgChildNode();

protected:
  // ...........................................................protected-ivars
};
#endif // __vsgChildNode_h
