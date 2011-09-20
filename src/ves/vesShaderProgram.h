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

#ifndef __vesShaderProgram_h
#define __vesShaderProgram_h

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

#include <list>
#include <map>
#include <string>

#include "vesGMTL.h"

#include "Painter.h"

#include <vsg/Shader/vsgShaderNode.h>

using namespace std;

class vesShaderProgram : public vsgShaderNode
{
public:

  // Ease of use.
  typedef std::map<string, unsigned int> AttributeBindingMap;

  enum AttributeIndex
  {
    Position            = 0,
    Normal              = 1,
    TextureCoordinate   = 2,
    Color               = 3,
    Scalar              = 4,
    CountAttributeIndex = 5
  };

   vesShaderProgram();
  ~vesShaderProgram();

  /*!
   * Add shader to a program. For now lifetime of the shader is controlled
   * by the program.
   */
  bool addShader(vesShader *shader);

  bool addBindAttributeLocation(const std::string& name, unsigned int location);

  bool validate();
  void use();

  int uniform(string value);
  int attribute(string value);

  void deleteProgram();

  void setUniformMatrix4x4f(string str, vesMatrix4x4f& mat);
  void setUniformMatrix3x3f(string str, vesMatrix3x3f& mat);
  void setUniformVector3f(string str, vesVector3f point);
  void setUniformVector2f(string str, vesVector2f point);
  void setUniformFloat(string str, float value);
  void setUniformInt(string str, int value);

  void enableVertexArray  (unsigned int location);
  void disableVertexArray (unsigned int location);

  bool link();

  void cleanUp();
  void deleteVertexAndFragment();

  virtual bool read(){;}

  virtual void render(Painter *render);


protected:

  void bindAttributes();
  void bindUniforms();


private:

  GLuint m_programHandle;

  static std::string preDefinedAttributeNames[CountAttributeIndex];

  std::list<vesShader*>    m_shaders;

  AttributeBindingMap      m_attributes;

  vesShaderProgram(const vesShaderProgram&);
  void operator=  (const vesShaderProgram&);
};


#endif
