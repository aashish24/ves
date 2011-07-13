// ============================================================================
/**
 * @file   vsgViewpointNode.h
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
#ifndef __vsgViewpointNode_h
#define __vsgViewpointNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgBindableNode.h"

// -------------------------------------------------------------pre-defines

// -------------------------------------------------------------------class
class vsgViewpointNode: public vsgBindableNode
{
public:
  // ........................................................public-methods
  vsgViewpointNode();
  virtual ~vsgViewpointNode();
  InOutSF(centerOfRotation,SFVec3f)
  InOutSF(description, SFString)
  InOutSF(jump, SFBool)
  InOutSF(orientation, SFRotation)
  InOutSF(position, SFVec3f)
  InOutSF(retainUserOffsets,SFBool)
protected:
  // .......................................................protected-ivars

};

#endif // __vsgViewpointNode_h
