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
  vtkSetGetMacro(View,vtkMatrix4x4f)
  vtkSetGetMacro(Model,vtkMatrix4x4f)
  vtkSetGetMacro(Projection,vtkMatrix4x4f)
protected:
  // ...........................................................protected-ivars
  vtkMatrix4x4f Projection,Model,View;
  std::vector<vtkMatrix4x4f> MatrixStack;
  // vtkMatrix4x4f MatrixStack[10];
  // int index;
  vtkMatrix4x4f Eval();
  void Push(vtkMatrix4x4f mat);
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
