/*=========================================================================

 Program:   Visualization Toolkit
 Module:    Shader.fsh

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

// Uniforms.
uniform lowp int primitiveType;

// Varying attributes.
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

void main()
{
  lowp vec4 color = vec4(0.95, 0.55, 0.5, 1.0);

  // 3 is line.
  if (primitiveType != 3) {
    mediump float intensity;

    mediump vec3 n = normalize(varNormal);

    intensity = dot(varLightDirection, n);

    if(intensity > 0.92) {
      color = vec4(0.95, 0.55, 0.5, 1.0);
    }
    else if(intensity > 0.70) {
      color = vec4(0.7, 0.4, 0.33, 1.0);
    }
    else if(intensity > 0.5) {
      color = vec4(0.6, 0.33, 0.26, 1.0);
    }
    else {
      color = vec4(0.4, 0.25, 0.20, 1.0);
    }
  }

  gl_FragColor = color;
}
