/*=========================================================================

 Program:   Visualization Toolkit
 Module:    Shader.vsh

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

uniform mat4   modelViewProjectionMatrix;     // model-view-projection matrix
uniform mat3   normalMatrix;  // normal matrix
uniform vec3   u_ecLightDir;     // light direction in eye coordinates
uniform vec2   u_scalarRange;

attribute vec4 vertexPosition;         // vertex position
attribute vec3 a_normal;         // vertex normal
attribute vec4 a_texcoord;       // texture coordinates
attribute float a_scalar;

varying vec4 v_texcoord;
varying vec2 v_tcoord;
//varying vec2 screenPos;

void main()
{
  //vec3 ec_normal = normalize(u_normalMatrix * a_normal);
  //float v_diffuse  = max(dot(u_ecLightDir,ec_normal),0.0);
  //vec3 outColor = vec3(.25,.25,.25)+v_diffuse * a_texcoord.xyz ;

  //v_texcoord = vec4(outColor, a_texcoord.a);
  float val = (a_scalar - u_scalarRange.x) / (u_scalarRange.y - u_scalarRange.x);
  //v_texcoord = vec4(a_scalar, a_scalar, a_scalar, 1);
  v_tcoord = vec2(a_scalar, 0.5);
  gl_Position = modelViewProjectionMatrix * vertexPosition;
  gl_PointSize = 1.0;
  //vec2 halfsize = vec2(480.0, 854.0) * 0.5;
  //screenPos = halfsize + ((gl_Position.xy / gl_Position.w) * halfsize);
}
