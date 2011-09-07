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

uniform mat4   modelViewProjectionMatrix;
uniform mat3   normalMatrix;  
uniform vec3   lightDirection;
uniform float  opacity;
uniform bool   enableDiffuse;

attribute vec4 vertexPosition;       
attribute vec3 vertexNormal;
attribute vec3 vertexColor;

varying lowp vec4  varVertexColor;
varying lowp vec4  varDiffuseColor;
varying lowp vec4  varAmbientColor;

varying lowp float varOpacity;
varying highp vec4 varPosition;
varying highp vec3 varNormal;
varying highp vec3 varLightDirection;

void main()
{
  // Default ambient color for now.
  varAmbientColor = vec4(0.01, 0.01, 0.01, 0.0);
  
  // Default diffuse color for now.
  varDiffuseColor = vec4(0.1, 0.1, 0.1, 0.0);
    
  // Transform vertex normal into eye space.
  varNormal = normalize(normalMatrix * vertexNormal);

  // Save light direction (direction light for now)
  varLightDirection = normalize(lightDirection);
    
  // Save opacity for shading later.
  varOpacity = opacity;
  
  // Save vertex color for shading later.
  varVertexColor = vec4(vertexColor, 0.0);
    
  // Save position for shading later.
  varPosition = modelViewProjectionMatrix * vertexPosition;
  
  // GLSL still requires this.
  gl_Position = varPosition;
}
