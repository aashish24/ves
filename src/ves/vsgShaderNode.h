/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgShaderNode.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgShaderNode - Abstract base type for all nodes that specify a programmable shader.
// .SECTION Description
// vsgShaderNode

#ifndef __vsgShaderNode_h
#define __vsgShaderNode_h
// --------------------------------------------------------------------includes
#include "vsgAppearanceChildNode.h"
#include <string>

// -----------------------------------------------------------------pre-defines
class vsgShaderNodeInternal;

// -----------------------------------------------------------------------class
class vsgShaderNode :public vsgAppearanceChildNode
{
public:
  // ............................................................public-methods
  vsgShaderNode();
  ~vsgShaderNode();
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
  vsgShaderNodeInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vsgShaderNode_h
