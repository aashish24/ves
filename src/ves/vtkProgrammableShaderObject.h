/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProgrammableShaderObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkProgrammableShaderObject - Abstract base type for all nodes that specify arbitary fields for interfacing with per-object attribute values
// .SECTION Description
// vtkProgrammableShaderObject concrete node instance can be used to program behavior for a shader in a scene.

#ifndef __vtkProgrammableShaderObject_h
#define __vtkProgrammableShaderObject_h
// --------------------------------------------------------------------includes

// -----------------------------------------------------------------pre-defines
class vtkProgrammableShaderObjectInternal;

// -----------------------------------------------------------------------class
class vtkProgrammableShaderObject
{
public:
  // ............................................................public-methods
  vtkProgrammableShaderObject();
  ~vtkProgrammableShaderObject();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vtkProgrammableShaderObjectInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vtkProgrammableShaderObject_h
