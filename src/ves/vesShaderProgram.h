//
//  vesShaderProgram.h
//  kiwi
//
//  Created by kitware on 4/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vesShaderProgram_h
#define __vesShaderProgram_h

#ifdef ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#endif
#include <map>
#include <vector>
#include <string>
#include "vesGMTL.h"
#include "Painter.h"

using namespace std;

// ----------------------------------------------------------------------struct
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

// -----------------------------------------------------------------------macro
#define VTK_STR_LIST_CNSTR_DECLARE(NAME) \
vtkStringList NAME (string value);

VTK_STR_LIST_CNSTR_DECLARE(_att)
VTK_STR_LIST_CNSTR_DECLARE(_uni)

// -----------------------------------------------------------------------class
class vesShaderProgram
{
public:
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

