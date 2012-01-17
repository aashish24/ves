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
/// \file vesToonShader_frag.glsl
///
/// \ingroup shaders

// Uniforms.
uniform lowp int primitiveType;

// Varying attributes.
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

void main()
{
  lowp vec4 color = vec4(0.95, 0.55, 0.5, 1.0);

  // 3 is line.
  if (primitiveType != 3) {
    mediump float intensity;

    mediump vec3 n = normalize(varNormal);

    intensity = dot(varLightDirection, n);

    if(intensity > 0.92) {
      color = vec4(0.95, 0.55, 0.5, 1.0);
    }
    else if(intensity > 0.70) {
      color = vec4(0.7, 0.4, 0.33, 1.0);
    }
    else if(intensity > 0.5) {
      color = vec4(0.6, 0.33, 0.26, 1.0);
    }
    else {
      color = vec4(0.4, 0.25, 0.20, 1.0);
    }
  }

  gl_FragColor = color;
}
