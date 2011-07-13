/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgAppearanceChildNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgAppearanceChildNode - Base node for all child nodes of vsgAppearanceNode type
// .SECTION Description
// vsgAppearanceChildNode Base node for all child nodes of vsgAppearanceNode type.

#ifndef __vsgAppearanceChildNode_h
#define __vsgAppearanceChildNode_h
// --------------------------------------------------------------------includes
#include "vsgNode.h"

// -----------------------------------------------------------------pre-defines
class vsgAppearanceChildNodeInternal;

// -----------------------------------------------------------------------class
class vsgAppearanceChildNode : public vsgNode
{
public:
  // ............................................................public-methods
  vsgAppearanceChildNode();
  ~vsgAppearanceChildNode();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vsgAppearanceChildNodeInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vsgAppearanceChildNode_h
