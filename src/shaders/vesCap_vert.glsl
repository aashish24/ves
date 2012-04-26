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
/// \file vesCapping_vert.glsl
///
/// \ingroup shaders

// Uniforms.
uniform bool         hasVertexColors;
uniform highp mat4   modelViewMatrix;
uniform mediump mat3 normalMatrix;
uniform highp mat4   projectionMatrix;
uniform highp vec4   clipPlaneEquation;

// Vertex attributes.
attribute highp vec3 vertexPosition;
attribute mediump vec3 vertexNormal;
attribute lowp vec4 vertexColor;

// Varying attributes.
varying lowp vec4  frontfaceColor;
varying lowp vec4  backfaceColor;

varying highp float clipDistance;

void main()
{
  lowp vec4 diffuseColor = vec4(1.0, 1.0, 1.0, 1.0);
  lowp vec4 ambientColor = vec4(0.1, 0.1, 0.1, 0.0);

  if (hasVertexColors) {
    frontfaceColor = vertexColor;
    backfaceColor = vertexColor;
  }
  else {
    frontfaceColor = diffuseColor;
    backfaceColor = diffuseColor;
  }

  // Save position for later use
  highp vec4 position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // Transform vertex normal into eye space
  mediump vec3 normal = normalize(normalMatrix * vertexNormal);

  // Save light direction (direction light for now)
  mediump vec4 lightSourcePos = vec4(-1000.0, 0.0, 0.650, 1.0);
  mediump vec3 lightDirection = normalize(lightSourcePos.xyz);

  // Front face lighting calculation
  lowp float nDotLFF = max(dot(normal, lightDirection), 0.0);
  frontfaceColor = vec4(frontfaceColor.xyz * nDotLFF, frontfaceColor.w);
  frontfaceColor += ambientColor;

  // Back face lighting calculation
  // Backface lighting is calculated using clip plane normal
  mediump vec3 planeNormal = -normalize(clipPlaneEquation.xyz);
  // Transform plane normal into eye space.
  planeNormal = normalize(normalMatrix * planeNormal);
  lowp float nDotLBF = max(dot(planeNormal, lightDirection), 0.0);
  backfaceColor = vec4(backfaceColor.xyz * nDotLBF, backfaceColor.w);
  backfaceColor += ambientColor;

  // Calculate clip distance
  clipDistance = dot(vertexPosition.xyz, clipPlaneEquation.xyz) + clipPlaneEquation.w;

  // GLSL still requires this
  gl_Position = position;
}
