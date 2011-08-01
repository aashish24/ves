// ============================================================================
/**
 * @file   vsgBackgroundNode.cpp
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
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
# include "vsg/vsg/vsgBackgroundNode.h"
// --------------------------------------------------------------------includes

namespace vsg {
  // -------------------------------------------------------------------macro

  // -------------------------------------------------------------------cnstr
  vsgBackgroundNode::vsgBackgroundNode()
  {
    _transparency = 0;
  }

  // -------------------------------------------------------------------destr
  vsgBackgroundNode::~vsgBackgroundNode()
  {
  }
  // ------------------------------------------------------------------public
  // -----------------------------------------------------------------private
}
