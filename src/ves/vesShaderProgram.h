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
    POSITION              = 0,
    NORMAL                = 1,
    TEXTURE_COORDINATE    = 2,
    COLOR                 = 3,
    SCALAR                = 4,
    COUNT_ATTRIBUTE_INDEX = 5
  };

   vesShaderProgram();
  ~vesShaderProgram();

  bool AddShader(vesShader *shader);

  bool AddBindAttributeLocation(const std::string& name, unsigned int location);

  bool Validate();
  void Use();

  int GetUniform(string value);
  int GetAttribute(string value);

  void DeleteProgram();

  void SetUniformMatrix4x4f(string str, vesMatrix4x4f& mat);
  void SetUniformMatrix3x3f(string str, vesMatrix3x3f& mat);
  void SetUniformVector3f(string str, vesVector3f point);
  void SetUniformVector2f(string str, vesVector2f point);
  void SetUniformFloat(string str, float value);
  void SetUniformInt(string str, int value);

  void EnableVertexArray  (unsigned int location);
  void DisableVertexArray (unsigned int location);

  bool Link();

  void Delete();
  void DeleteVertexAndFragment();

  virtual bool read(){;}

  virtual void render(Painter *render);


protected:

  void BindAttributes();
  void BindUniforms();


private:

  GLuint ProgramHandle;

  static std::string PreDefinedAttributeNames[COUNT_ATTRIBUTE_INDEX];

  std::list<vesShader*>    Shaders;

  AttributeBindingMap      Attributes;
};


#endif
