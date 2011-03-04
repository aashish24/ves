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

uniform mat4   u_mvp_matrix;
uniform mat3   u_normal_matrix;

attribute vec4 a_color;

attribute vec4 position;
attribute vec3 normal;
attribute vec4 color;

varying vec4 colorVarying;

uniform float translate;

void main()
{
  gl_Position = u_mvp_matrix * position;

  vec3 norm = u_normal_matrix * normal;
  vec3 light = normalize(vec3(0.0, 0.4, 1.0));
  vec3 eye = vec3(0, 0, 1);
  vec3 EL = normalize(light + eye);

  float df = max(0.0, dot(norm, light));
  //float sf = max(0.01, dot(norm, EL));
  //sf = pow(sf, 5.0)

  vec3 outColor = vec3(0.01, 0.01, 0.01) + df * a_color.xyz;// + sf * vec3(0.9, 0.9, 0.9);

  colorVarying = vec4(outColor, 1.0);
}
