/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Painter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Painter - Paints the scene graph
// .SECTION Description
// Painter

#ifndef __Painter_h
#define __Painter_h
// --------------------------------------------------------------------includes
// #include "Transform.h"
# include "Shape.h"
# include "vesActorCollection.h"
# include "vesActor.h"
# include "vesShader.h"
# include "vesMapper.h"
# include "vesCamera.h"

#include <vector>
// -----------------------------------------------------------------pre-defines
class PainterInternal;

// -----------------------------------------------------------------------class
class Painter
{
public:
  // ............................................................public-methods
  Painter();
  ~Painter();
  // void Transform(Transform* transform);
  void visitShape(Shape* shape);
  void Shader(vesShader * shader);
  void Mapper(vesMapper *mapper);
  void Actor(vesActor * actor);
  void ActorCollection(vesActorCollection *actor);
  void Camera(vesCamera *camera);
  void ShaderProgram(vesShaderProgram *shaderProg);
  void Push(vesMatrix4x4f mat);
  void Pop();
  
protected:
  // ...........................................................protected-ivars
  std::vector<vesMatrix4x4f> MatrixStack;
  vesMatrix4x4f Eval();
  float Aspect, NearZ, FarZ;
protected:
//BTX
  // .......................................................................BTX

private:
  PainterInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __Painter_h
