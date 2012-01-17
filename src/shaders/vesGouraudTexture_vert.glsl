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
/// \file vesGouraudTexture_vert.glsl
///
/// \ingroup shaders

uniform highp mat4 modelViewMatrix;
uniform highp mat4 projectionMatrix;
uniform mediump mat3 normalMatrix;

attribute highp vec4 vertexPosition;
attribute mediump vec3 vertexNormal;
attribute mediump vec4 vertexTextureCoordinate;

varying mediump vec2 textureCoordinate;
//varying mediump vec4 varColor;
varying highp float nDotL;

void main()
{



  // Default ambient color for now.
  //lowp vec4 ambientColor = vec4(0.01, 0.01, 0.01, 0.0);
  //lowp vec4 diffuseColor = vec4(0.1, 0.1, 0.1, 0.0);
  //varColor = vec4(1.0, 1.0, 1.0, 0.0);;


  lowp vec3 normal = normalize(normalMatrix * vertexNormal);
  lowp vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.650));

  //lowp float nDotL = max(dot(normal, lightDirection), 0.0);
  //varColor = vec4(varColor.xyz * nDotL, varColor.w);

  nDotL = max(dot(normal, lightDirection), 0.0);


  gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;

  textureCoordinate = vertexTextureCoordinate.xy;
}
