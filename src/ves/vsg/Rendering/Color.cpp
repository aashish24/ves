// ============================================================================
/**
 * @file   Color.cpp
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
# include "Color.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"

namespace vsg {
    // -------------------------------------------------------------------macro

    // ................................................................internal
    // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
    // be put in the class declaration. For all newly defined pointers make sure
    // to update constructor and destructor methods.
    struct ColorInternal
    {
      double value; // sample

    };
    // ................................................................internal

    // -------------------------------------------------------------------cnstr
    Color::Color()
       {
         _internal = new ColorInternal();
       }

    // -------------------------------------------------------------------destr
    Color::~Color()
       {
         delete _internal;
       }

    // ------------------------------------------------------------------public
    // -----------------------------------------------------------------private
}

bool vsg::Color::accept(vsgVisitor *vsgVisitor)
{
  return vsgVisitor->visitColor(this);
}
