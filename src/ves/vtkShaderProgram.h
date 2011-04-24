//
//  vtkShaderProgram.h
//  kiwi
//
//  Created by kitware on 4/22/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vtkShaderProgram__
#define __vtkShaderProgram__

#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <map>
#include <vector>
#include <string>

using namespace std;

struct cmp_str
{
  bool operator()(char const *a, char const *b)
  {
    return std::strcmp(a, b) < 0;
  }
};

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

struct vtkShaderProgram
{
  vtkShaderProgram(char* vertexShaderStr,
                   char* fragmentShaderStr,
                   vtkStringList uniforms,
                   vtkStringList attributes);
  ~vtkShaderProgram();
  bool Validate();
  void Run()
  {
    glUseProgram(this->Program);
  }
  
  int GetUniform(string value)
  {
    return glGetUniformLocation(this->Program, value.c_str());
  }
  
  int GetAttribute(string value)
  {
    return glGetAttribLocation(this->Program, value.c_str());
  }
  
  void DeleteProgram()
  {
    if (this->Program)
    {
      glDeleteProgram(this->Program);
      this->Program = 0;
    } 
  }
  
  void CompileAndLoadVertexShader(char* vertexShaderStr);
  void CompileAndLoadFragmentShader(char* fragmentShaderStr);
  void BindAttributes(vtkStringList attribs);
  void BindUniforms(vtkStringList uniforms);
  bool Link();
  void Delete();
  void DeleteVertexAndFragment();
  GLuint CompileShader(GLenum type, const char* source);
  
  GLuint Program;
  GLuint VertexShader;
  GLuint FragmentShader;
  map<string,int> Uniforms;
  map<string,int> Attributes;
};

vtkShaderProgram _program(char* vertexShaderStr,
                           char* fragmentShaderStr,
                           vtkStringList uniforms,
                           vtkStringList attributes);

#endif
//struct vtkShaderAttributes
//{
//  map<char*, int> Attributes;
//  friend vtkShaderAttributes operator ,(vtkShaderAttributes a,vtkShaderAttributes b)
//  {
//    vtkShaderAttributes temp;
//    temp = a;
//    for( map<char *, int, cmp_str>::iterator ii=b.Attributes.begin(); ii!=b.Attributes.end(); ++ii)
//    {
//      temp.Attributes[(*ii).first] = (*ii).second;
//    }
//    return temp;
//  }
//};
//
//vtkShaderAttributes _attrib(char* key,int value)
//{
//  vtkShaderAttributes temp;
//  //temp.Uniform[key] = value;
//  temp.Attributes.insert(std::pair<char *,int>(key,value));
//  return temp;
//}
//
//struct vtkShaderUniforms
//{
//  map<char*,int> Uniforms;
//  friend vtkShaderUniforms operator ,(vtkShaderUniforms a,vtkShaderUniforms b)
//  {
//    vtkShaderUniforms temp;
//    temp = a;
//    for( map<char *, int, cmp_str>::iterator ii=b.Uniforms.begin(); ii!=b.Uniforms.end(); ++ii)
//    {
//      temp.Uniforms[(*ii).first] = (*ii).second;
//    }
//    return temp;
//  }
//};
//
//vtkShaderUniforms _uniform(char* key,int value)
//{
//  vtkShaderUniforms temp;
//  //temp.Uniform[key] = value;
//  temp.Uniforms.insert(std::pair<char *,int>(key,value));
//  return temp;
//}

