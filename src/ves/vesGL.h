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
#ifndef VESGL_H
#define VESGL_H

#ifdef ANDROID
# include <GLES2/gl2.h>
# include <GLES2/gl2ext.h>
#else
# include <OpenGLES/ES2/gl.h>
# include <OpenGLES/ES2/glext.h>
#endif

#ifndef GL_SAMPLER_1D
    #define GL_SAMPLER_1D               0x8B5D
    #define GL_SAMPLER_2D               0x8B5E
    #define GL_SAMPLER_3D               0x8B5F
    #define GL_SAMPLER_1D_SHADOW        0x8B61
    #define GL_SAMPLER_2D_SHADOW        0x8B62
#endif


#define printGLError(os)        \
  GLenum error = glGetError();  \
  if (error != GL_NO_ERROR) {   \
  os << "ERROR: Occured in " << __FILE__ << " at line " << __LINE__ << " (error code: " << error << ")\n";   \
  }


#define printGLErrorWithMessage(message, os)   \
  GLenum error = glGetError();      \
  if (error != GL_NO_ERROR) {       \
    os << message << "\n";          \
    os << "ERROR: Occured in " << __FILE__ << " at line " << __LINE__ << " (error code: " << error << ")\n";   \
  }


#endif
