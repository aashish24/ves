// ============================================================================
/**
 * @file   ClipPlane.cpp
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
# include "ClipPlane.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"

namespace vsg {
    // -------------------------------------------------------------------macro

    // ................................................................internal
    // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
    // be put in the class declaration. For all newly defined pointers make sure
    // to update constructor and destructor methods.
    struct ClipPlaneInternal
    {
      double value; // sample

    };
    // ................................................................internal

    // -------------------------------------------------------------------cnstr
    ClipPlane::ClipPlane()
    {
      _internal = new ClipPlaneInternal();
      _enabled = true;
      _plane[0] = 0;
      _plane[1] = 1;
      _plane[2] = 0;
      _plane[3] = 0;
    }

    // -------------------------------------------------------------------destr
    ClipPlane::~ClipPlane()
       {
         delete _internal;
       }

    // ------------------------------------------------------------------public
    // -----------------------------------------------------------------private
}

bool vsg::ClipPlane::accept(vsgVisitor *vsgVisitor)
{
  return vsgVisitor->visitClipPlane(this);
}
