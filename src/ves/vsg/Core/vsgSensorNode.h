// ============================================================================
/**
 * @file   vsgSensorNode.h
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
#ifndef __vsgSensorNode_h
#define __vsgSensorNode_h
// --------------------------------------------------------------------includes
# include "vsgMacro.h"
# include "vsgTypes.h"
# include "vsgChildNode.h"

// -----------------------------------------------------------------pre-defines

// -----------------------------------------------------------------------class
class vsgSensorNode : public vsgChildNode
{
public:
  // ............................................................public-methods
  vsgSensorNode();
  ~vsgSensorNode();
  SFBool isActive();
  InOutSF(enabled,SFBool)

  protected:
  // ...........................................................protected-ivars
};

#endif // __vsgSensorNode_h
