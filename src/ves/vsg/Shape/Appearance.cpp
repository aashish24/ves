// ============================================================================
/**
 * @file   Appearance.cpp
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
#include "vsg/Shape/Appearance.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"
#include "Painter.h"

namespace vsg {
    // -------------------------------------------------------------------macro

    // ................................................................internal
    // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
    // be put in the class declaration. For all newly defined pointers make sure
    // to update constructor and destructor methods.
    struct AppearanceInternal
    {
      double value; // sample

    };
    // ................................................................internal

    // -------------------------------------------------------------------cnstr
    Appearance::Appearance()
       {
         _internal = new AppearanceInternal();
       }

    // -------------------------------------------------------------------destr
    Appearance::~Appearance()
       {
         delete _internal;
       }

    // ------------------------------------------------------------------public
    // -----------------------------------------------------------------private
}

bool vsg::Appearance::accept(vsgVisitor* vsgVisitor)
{
  return vsgVisitor->visitAppearance(this);
}

void vsg::Appearance::Render(Painter* render)
{
  this->_shaders[0]->Render(render);
}

bool vsg::Appearance::Read()
{
  this->_shaders[0]->Read();
  return true;
}


