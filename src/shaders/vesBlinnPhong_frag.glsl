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
varying mediump vec4 varPosition;
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

varying lowp vec4 varColor;

void main()
{
  lowp vec4 color = varColor;

  // 3 is line.
  if (primitiveType != 3) {
    lowp float nDotL;
    lowp float nDotH;

    highp vec3 n = normalize(varNormal);

    // Default to metallic look and feel.
    highp float specularShininess = 64.0;
    lowp vec4  specularColor = vec4(0.6, 0.6, 0.6, 1.0);

    highp vec3  viewDirection = normalize(-varPosition.xyz);

    // Using half vector for specular lighting as it is much cheaper than
    // calculating reflection vector.
    highp vec3 halfVector = normalize(varLightDirection + viewDirection);

    nDotL = max(dot(n, normalize(varLightDirection)), 0.0);

    nDotH = max(dot(n, halfVector), 0.1);
    color = color * nDotL + specularColor * pow(nDotH, specularShininess);
  }

  color.w = 1.0;

  gl_FragColor = color;
}
