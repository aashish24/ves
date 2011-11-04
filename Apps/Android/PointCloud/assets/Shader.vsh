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

uniform mat4   modelViewMatrix;     // model-view matrix
uniform mat4   projectionMatrix;
uniform vec2   u_scalarRange;

attribute vec4 vertexPosition;   // vertex position
attribute vec4 a_texcoord;       // texture coordinates
attribute float a_scalar;

varying vec4 v_texcoord;
varying vec2 v_tcoord;

void main()
{
  float val = (a_scalar - u_scalarRange.x) / (u_scalarRange.y - u_scalarRange.x);
  v_tcoord = vec2(a_scalar, 0.5);
  gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;
  gl_PointSize = 1.0;
}
