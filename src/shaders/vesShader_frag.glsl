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
/// \file vesShader_frag.glsl
///
/// \ingroup shaders

#ifdef GL_ES_VERSION_2_0
    #version 100
#else
    #version 120
#endif

#ifndef GL_ES_VERSION_2_0
    // Ignore GLES 2 precision specifiers
    #define lowp
    #define mediump
    #define highp
#endif

varying lowp vec4 varColor;

void main()
{
  gl_FragColor = varColor;
}
