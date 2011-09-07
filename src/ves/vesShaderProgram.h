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
#include <map>
#include <vector>
#include <string>
#include "vesGMTL.h"
#include "Painter.h"

using namespace std;

struct vtkStringList
{
  vector<std::string> List;
  friend vtkStringList operator, (vtkStringList a, vtkStringList b)
  {
    vtkStringList temp = a;
    for(int i=0;i<b.List.size();++i)
    {
      temp.List.push_back(b.List[i]);
    }
    return temp;
  }
};

#define VTK_STR_LIST_CNSTR_DECLARE(NAME) \
  vtkStringList NAME (string value);

VTK_STR_LIST_CNSTR_DECLARE(_att)
VTK_STR_LIST_CNSTR_DECLARE(_uni)

class vesShaderProgram
{
public:

  /**
   * vesShaderProgram constructor.
   * \todo change arguments from char* to std::string
   */
  vesShaderProgram(char* vertexShaderStr,
                   char* fragmentShaderStr,
                   vtkStringList uniforms,
                   vtkStringList attributes);
  ~vesShaderProgram();
  void Render(Painter *render);
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
  void EnableVertexArray(string str);
  void DisableVertexArray(string str);
  void CompileAndLoadVertexShader(char* vertexShaderStr);
  void CompileAndLoadFragmentShader(char* fragmentShaderStr);
  void BindAttributes(vtkStringList attribs);
  void BindUniforms(vtkStringList uniforms);
  bool Link();
  void Delete();
  void DeleteVertexAndFragment();
  GLuint CompileShader(GLenum type, const char* source);
private:
  GLuint Program;
  GLuint VertexShader;
  GLuint FragmentShader;
  map<string,int> Uniforms;
  map<string,int> Attributes;
};

vesShaderProgram _program(char* vertexShaderStr,
                          char* fragmentShaderStr,
                          vtkStringList uniforms,
                          vtkStringList attributes);

#endif
