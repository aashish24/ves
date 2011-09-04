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

#ifndef __Painter_h
#define __Painter_h

#include "vesGMTL.h"

#include <vector>

namespace vsg
{
class Shape;
}

class vesActor;
class vesActorCollection;
class vesCamera;
class vesMapper;
class vesShader;
class vesShaderProgram;
class vesTexture;
class PainterInternal;

/**
 * \class Painter
 * \brief Paints the scene graph.
 */

class Painter
{
public:
  Painter();
  ~Painter();
  void Texture(vesTexture* textureBackground);
  void visitShape(vsg::Shape* shape);
  void Shader(vesShader * shader);

  /** Render the supplied actor. */
  void Actor(vesActor * actor);

  /** Render all actors in the supplied collection. */
  void ActorCollection(vesActorCollection *actor);

  /** Set the camera as the active camera, modifying the model-view matrix. */
  void setCamera(vesCamera *camera);

  /** Set the supplied shaeder program as the active program. */
  void setShaderProgram(vesShaderProgram *shaderProgram);

  /**
   * Push the supplied matrix to the top of the stack, this will not post
   * multiply the current stack.
   * \param matrix Will be added to the stack of matrices to be multipled.
   */
  void push(const vesMatrix4x4f& matrix);

  /** Pop the current matrix from the top of the stack. */
  void pop();

  /** Set the background texture to the supplied texture. */
  void setBackgroundTexture(vesTexture* background);

protected:
  vesMatrix4x4f eval(int startIndex);

  std::vector<vesMatrix4x4f> m_matrixStack;
  float m_aspect, m_nearZ, m_farZ;
  vesTexture* m_textureBackground;

private:
  PainterInternal *m_internal;
};

#endif // __Painter_h
