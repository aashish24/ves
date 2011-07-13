// ============================================================================
/**
 * @file   vsgBindableNode.h
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
 * X3DBindableNode is the abstract base type for all bindable children nodes, including Background, TextureBackground, Fog, NavigationInfo and Viewpoint. For complete discussion of bindable behaviors, see 7.2.2 Bindable children nodes.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgBindableNode_h
#define __vsgBindableNode_h
// --------------------------------------------------------------------includes
#include "vsg/Utility/vsgMacro.h"
#include "vsg/Utility/vsgTypes.h"
#include "vsg/Core/vsgChildNode.h"

// -------------------------------------------------------------pre-defines

// -------------------------------------------------------------------class
class vsgBindableNode: public vsgChildNode
{
public:
  // ........................................................public-methods
  vsgBindableNode();
  virtual ~vsgBindableNode();
  InOnlySF(bind, SFBool)
  OutOnlySF(bindTime,SFTime);
  SFBool isBound();
protected:
  // .......................................................protected-ivars
};
#endif // __vsgBindableNode_h
