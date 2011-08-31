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

uniform mat4   u_mvpMatrix;     // model-view-projection matrix
uniform mat3   u_normalMatrix;  // normal matrix
uniform vec3   u_ecLightDir;     // light direction in eye coordinates
uniform float  u_opacity;
uniform bool   u_enable_diffuse;

attribute vec4 a_vertex;         // vertex position
attribute vec3 a_normal;         // vertex normal
attribute vec3 a_vertex_color;

varying vec4 v_color;

void main()
{
  // put vertex normal into eye coords
  vec3 ec_normal = normalize(u_normalMatrix * a_normal);

  // compute diffuse scale factor
  float diffuse = max(dot(u_ecLightDir,ec_normal), 0.0);

  vec3 rgb = a_vertex_color;
  if (u_enable_diffuse) {
    rgb = vec3(.25,.25,.25) + diffuse * a_vertex_color;
  }

  v_color = vec4(rgb, u_opacity);

  gl_Position = u_mvpMatrix * a_vertex;
}
