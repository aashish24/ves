/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAppearanceChildNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAppearanceChildNode - Base node for all child nodes of vtkAppearanceNode type
// .SECTION Description
// vtkAppearanceChildNode Base node for all child nodes of vtkAppearanceNode type.

#ifndef __vtkAppearanceChildNode_h
#define __vtkAppearanceChildNode_h
// --------------------------------------------------------------------includes
#include "vtkNode.h"

// -----------------------------------------------------------------pre-defines
class vtkAppearanceChildNodeInternal;

// -----------------------------------------------------------------------class
class vtkAppearanceChildNode : public vtkNode
{
public:
  // ............................................................public-methods
  vtkAppearanceChildNode();
  ~vtkAppearanceChildNode();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkAppearanceChildNodeInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vtkAppearanceChildNode_h
