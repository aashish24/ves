// ============================================================================
/**
 * @file   vsgBackgroundNode.h
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
 * X3DBackgroundNode is the abstract type from which all backgrounds
 * inherit. X3DBackgroundNode is a bindable node that, when bound, defines the
 * panoramic background for the scene. For complete information on backgrounds,
 * see 24.2.1 Backgrounds.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgBackgroundNode_h
#define __vsgBackgroundNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgBindableNode.h"

namespace vsg {
  // -------------------------------------------------------------pre-defines

  // -------------------------------------------------------------------class
  class vsgBackgroundNode: public vsgBindableNode
  {
  public:
    // ........................................................public-methods
    vsgBackgroundNode();
    virtual ~vsgBackgroundNode();
    InOutMF(groundAngle,MFFloat)
    InOutMF(groundColor,MFColor)
    InOutMF(skyAngle,MFFloat)
    InOutMF(skyColor,MFColor)
    InOutSF(transparency,SFFloat)
  protected:
    // .......................................................protected-ivars

  };
}
#endif // __vsgBackgroundNode_h
