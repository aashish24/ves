/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
// .NAME Painter - Paints the scene graph
// .SECTION Description
// Painter

#ifndef __Painter_h
#define __Painter_h
// --------------------------------------------------------------------includes
// #include "Transform.h"
#include "vsg/Shape/Shape.h"
#include "vesActorCollection.h"
#include "vesActor.h"
#include "vesShader.h"
#include "vesMapper.h"
#include "vesCamera.h"
#include "vesTexture.h"
#include "vsg/EnvEffects/TextureBackground.h"

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
  void Texture(vesTexture* textureBackground);
  void visitShape(vsg::Shape* shape);
  void Shader(vesShader * shader);
  void Mapper(vesMapper *mapper);
  void Actor(vesActor * actor);
  void ActorCollection(vesActorCollection *actor);
  void Camera(vesCamera *camera);
  void ShaderProgram(vesShaderProgram *shaderProg);
  void Push(vesMatrix4x4f mat);
  void Pop();
  void SetBackgroundTexture(vesTexture* background);
protected:
  // ...........................................................protected-ivars
  std::vector<vesMatrix4x4f> MatrixStack;
  vesMatrix4x4f Eval(int startIndex);
  float Aspect, NearZ, FarZ;
  vesTexture* _textureBackground;

protected:
  //BTX
  // .......................................................................BTX

private:
  PainterInternal *Internal;

  //ETX
  // .......................................................................ETX
};

#endif // __Painter_h
