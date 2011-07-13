/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgProgrammableShaderObject.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vsgProgrammableShaderObject - Abstract base type for all nodes that specify arbitary fields for interfacing with per-object attribute values
// .SECTION Description
// vsgProgrammableShaderObject concrete node instance can be used to program behavior for a shader in a scene.

#ifndef __vsgProgrammableShaderObject_h
#define __vsgProgrammableShaderObject_h
// --------------------------------------------------------------------includes

// -----------------------------------------------------------------pre-defines
class vsgProgrammableShaderObjectInternal;

// -----------------------------------------------------------------------class
class vsgProgrammableShaderObject
{
public:
  // ............................................................public-methods
  vsgProgrammableShaderObject();
  ~vsgProgrammableShaderObject();

protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:
  vsgProgrammableShaderObjectInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vsgProgrammableShaderObject_h
