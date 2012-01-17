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
/// \file vesToonShader_vert.glsl
///
/// \ingroup shaders

// Uniforms.
uniform bool  hasVertexColors;
uniform mediump vec3 lightDirection;
uniform highp mat4 modelViewMatrix;
uniform mediump mat3 normalMatrix;
uniform lowp int   primitiveType;
uniform highp mat4 projectionMatrix;

// Vertex attributes.
attribute highp vec3 vertexPosition;
attribute mediump vec3 vertexNormal;

// Varying attributes.
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

void main()
{
  vec4 varPosition = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // 3 is line
  if (primitiveType != 3) {
    // Transform vertex normal into eye space.
    varNormal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    varLightDirection = normalize(vec3(0.0, 0.0, 0.650));
  }

  // GLSL still requires this.
  gl_Position = varPosition;
}
