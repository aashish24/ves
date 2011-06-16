/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkShaderNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkShaderNode - Abstract base type for all nodes that specify a programmable shader.
// .SECTION Description
// vtkShaderNode

#ifndef __vtkShaderNode_h
#define __vtkShaderNode_h
// --------------------------------------------------------------------includes
#include "vtkAppearanceChildNode.h"
#include <string>

// -----------------------------------------------------------------pre-defines
class vtkShaderNodeInternal;

// -----------------------------------------------------------------------class
class vtkShaderNode :public vtkAppearanceChildNode
{
public:
  // ............................................................public-methods
  vtkShaderNode();
  ~vtkShaderNode();
  void Activate(bool value);
  bool IsSelected();
  bool IsValid();
protected:
  // ...........................................................protected-ivars
  std::string Language;
protected:
//BTX
  // .......................................................................BTX
private:
  vtkShaderNodeInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vtkShaderNode_h
