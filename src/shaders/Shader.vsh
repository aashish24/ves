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

attribute vec4 a_vertex;         // vertex position
attribute vec3 a_normal;         // vertex normal
attribute vec4 a_texcoord;       // texture coordinates

varying float v_diffuse;
varying vec4  v_texcoord;

void main()
{
  // put vertex normal into eye coords
  vec3 ec_normal = normalize(u_normalMatrix * a_normal);
  
  // emit diffuse scale factor, texcoord and position
  v_diffuse  = max(dot(u_ecLightDir,ec_normal),0.0);
  
  //vec3 light = normalize(vec3(0.0, 0.0, -1.0));
  //vec3 eye = vec3(0, 0, 1);
  //vec3 EL = normalize(light + eye);

  //float df = max(0.0, dot(ec_normal, light));
  //float sf = max(0.01, dot(ec_normal, EL));
  //sf = pow(sf, 5.0)

  vec3 outColor = v_diffuse * a_texcoord.xyz;// + sf * vec3(0.9, 0.9, 0.9);

  v_texcoord = vec4(outColor, 1.0);
  //v_texcoord = a_texcoord;
  gl_Position = u_mvpMatrix * a_vertex;
}
