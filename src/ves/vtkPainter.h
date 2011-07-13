/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPainter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkPainter - Paints the scene graph
// .SECTION Description
// vtkPainter

#ifndef __vtkPainter_h
#define __vtkPainter_h
// --------------------------------------------------------------------includes
// #include "vtkTransform.h"
# include "vtkShape.h"
# include "vtkActorCollection.h"
# include "vtkActor.h"
# include "vtkShader.h"
# include "vtkMapper.h"
#include <vector>
// -----------------------------------------------------------------pre-defines
class vtkPainterInternal;

// -----------------------------------------------------------------------class
class vtkPainter
{
public:
  // ............................................................public-methods
  vtkPainter();
  ~vtkPainter();
  // void Transform(vtkTransform* transform);
  void Shape(vtkShape* shape);
  void Shader(vtkShader * shader);
  void Mapper(vtkMapper *mapper);
  void Actor(vtkActor * actor);
  void ActorCollection(vtkActorCollection *actor);
  void ShaderProgram(vtkShaderProgram *shaderProg);
  vesSetGetMacro(View,vesMatrix4x4f)
  vesSetGetMacro(Model,vesMatrix4x4f)
  vesSetGetMacro(Projection,vesMatrix4x4f)
protected:
  // ...........................................................protected-ivars
  vesMatrix4x4f Projection,Model,View;
  std::vector<vesMatrix4x4f> MatrixStack;
  // vesMatrix4x4f MatrixStack[10];
  // int index;
  vesMatrix4x4f Eval();
  void Push(vesMatrix4x4f mat);
  void Pop();

protected:
//BTX
  // .......................................................................BTX

private:
  vtkPainterInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __vtkPainter_h
