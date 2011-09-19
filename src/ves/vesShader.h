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
// .NAME vesShader - Concrete class which implementes ProgramShader
// .SECTION Description
// vesShader [refer ISO/IEC 19775-1:2008 31.4.6]

#ifndef __vesShader_h
#define __vesShader_h

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

#include <string>

class vesShader
{
 public:
 enum ShaderType
 {
     VERTEX         = GL_VERTEX_SHADER,
//     TESSCONTROL    = GL_TESS_CONTROL_SHADER,
//     TESSEVALUATION = GL_TESS_EVALUATION_SHADER,
//     GEOMETRY       = GL_GEOMETRY_SHADER_EXT,
     FRAGMENT       = GL_FRAGMENT_SHADER,
     UNDEFINED      = -1
 };

 vesShader(ShaderType type = UNDEFINED);
 vesShader(ShaderType type, const std::string &source);

 // \todo: Support shader binaries.

 // \todo: Implement these useful functions.
 // int Compare(const vesShader &rhs);

 bool SetShaderType(ShaderType type);

 inline ShaderType GetShaderType() const
   {
   return Type;
   }

//  std::string GetShaderTypeName() const;

 inline void SetFileName(const std::string &fileName)
   {
     this->ShaderFileName = fileName;
//     this->Modified();
   }

 inline const std::string& GetFileName() const
   {
   return this->ShaderFileName;
   }

 inline void SetShaderSource(const std::string &sourceText)
   {
   this->ShaderSource = sourceText;
//   this->Modified();
   }

 inline const std::string& GetShaderSource() const
   {
   return this->ShaderSource;
   }

 bool LoadShaderSourceFromFile(const std::string& fileName);

 // If needed, compile the PC shader.
 void CompileShader();

 // For a given GL context, attach a glShader to a glProgram.
 // \todo: Improvement (support per context shader).
// void AttachShader(unsigned int contextID, GLuint program) const;

 // Attach a glShader to a glProgram.
 void AttachShader(GLuint program) const;

 // For a given GL context, detach a glShader to a glProgram.
 // \todo: Implement this.
 // void DetachShader(unsigned int contextID, GLuint program) const;

 // Query InfoLog from a glShader.
 // \todo: Implement this.
 // bool GetGlShaderInfoLog(unsigned int contextID, std::string& log) const;

 // Mark internal glShader for deletion.
 // Deletion requests are queued until they can be executed
 // in the proper GL context.
 // \todo: Implement this.
 // static void DeleteGlShader(unsigned int contextID, GLuint shader);

 // flush all the cached glShaders which need to be deleted
 //  in the OpenGL context related to contextID.
 // \todo: Implement this.
 // static void flushDeletedGlShaders(unsigned int contextID,double currentTime, double& availableTime);


 protected:

   ShaderType Type;

   GLuint      ShaderHandle;

   std::string ShaderFileName;

   std::string ShaderSource;
};

#endif // __vesShader_h
