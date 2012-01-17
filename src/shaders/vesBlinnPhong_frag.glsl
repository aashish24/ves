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
/// \file vesBlinnPhong_frag.glsl
///
/// \ingroup shaders

// Uniforms.
uniform lowp int primitiveType;

// Varying attributes.
varying mediump vec4 varPosition;
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

varying lowp vec4 varColor;

void main()
{
  lowp vec4 color = varColor;

  // 3 is line.
  if (primitiveType != 3) {
    lowp float nDotL;
    lowp float nDotH;

    highp vec3 n = normalize(varNormal);

    // Default to metallic look and feel.
    highp float specularShininess = 64.0;
    lowp vec4  specularColor = vec4(0.6, 0.6, 0.6, 1.0);

    highp vec3  viewDirection = normalize(-varPosition.xyz);

    // Using half vector for specular lighting as it is much cheaper than
    // calculating reflection vector.
    highp vec3 halfVector = normalize(varLightDirection + viewDirection);

    nDotL = max(dot(n, normalize(varLightDirection)), 0.0);

    nDotH = max(dot(n, halfVector), 0.1);
    color = color * nDotL + specularColor * pow(nDotH, specularShininess);
  }

  color.w = 1.0;

  gl_FragColor = color;
}
