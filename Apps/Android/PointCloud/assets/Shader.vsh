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

uniform highp mat4   modelViewMatrix;     // model-view matrix
uniform highp mat4   projectionMatrix;
//uniform vec2   u_scalarRange;

attribute highp vec3 vertexPosition;   // vertex position
//attribute vec4 a_texcoord;       // texture coordinates
attribute highp float vertexColor;

varying highp vec2 v_tcoord;
//varying lowp vec3 color;

void main()
{
  //float val = (a_scalar - u_scalarRange.x) / (u_scalarRange.y - u_scalarRange.x);
  float a_scalar = vertexColor;
  v_tcoord = vec2(a_scalar, 0.5);
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);
  //gl_Position = vertexPosition;
  gl_PointSize = 1.0;
  //color = vec3(vertexColor, 0, 0);
}
