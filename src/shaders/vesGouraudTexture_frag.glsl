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
/// \file vesGouraudTexture_frag.glsl
///
/// \ingroup shaders

varying mediump vec2 textureCoordinate;

//varying mediump vec4 varColor;
varying highp float nDotL;

uniform highp sampler2D image;

void main()
{

  //highp vec4 diffuse = vec4(0.1, 0.1, 0.1, 1.0)
  //gl_FragColor = (texture2D(image, textureCoordinate) + diffuse) * nDotL;

  highp vec4 color = texture2D(image, textureCoordinate) * nDotL;
  gl_FragColor = vec4(color.xyz, 1.0);
}
