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

uniform mediump float opacity;

uniform bool useToonShader;
uniform bool useBlinnPhongShader;

varying lowp vec4  varVertexColor;
varying lowp vec4  varDiffuseColor;
varying lowp vec4  varAmbientColor;

varying highp vec4 varPosition;
varying highp vec3 varNormal;
varying highp vec3 varLightDirection;

void main()
{
  highp vec3 n = normalize(varNormal);

  if(useBlinnPhongShader)
  {
    lowp float nDotL;
    lowp float nDotH;

    // Default to metallic look and feel.
    lowp float specularShininess = 128.0;
    lowp vec4  specularColor     = vec4(0.8, 0.8, 0.8, 0.0);

    lowp vec4 color = varAmbientColor;

    highp vec3  viewDirection = normalize(-varPosition.xyz);

    // Using half vector for specular lighting as it is much cheaper than
    // calculating reflection vector.
    highp vec3 halfVector = normalize(varLightDirection + viewDirection);

    nDotL = max(dot(n, normalize(varLightDirection)), 0.0);

    // Apply diffuse only for upper half dome.
    if(nDotL > 0.0)
    {
      nDotH = max(dot(n, halfVector), 0.0);

      color += (varDiffuseColor + varVertexColor) * nDotL +
               specularColor * pow(nDotH, specularShininess);
    }
    else
    {
      color += varVertexColor;
    }

    gl_FragColor = vec4(color.xyz, opacity);
  }
  else if(useToonShader)
  {
    highp float intensity;
    lowp vec4 color;

    intensity = dot(varLightDirection, n);

    if(intensity > 0.95)
    {
      color = vec4(0.95, 0.55, 0.5, 1.0);
    }
    else if(intensity > 0.75)
    {
      color = vec4(0.6, 0.22, 0.20, 1.0);
    }
    else if(intensity > 0.5)
    {
      color = vec4(0.4, 0.18, 0.16, 1.0);
    }
    else
    {
      color = vec4(0.36, 0.15, 0.12, 1.0);
    }

    gl_FragColor = color;
  }
  else
  {
    gl_FragColor = vec4(0.5, 0.5, 0.5, 1.0);
  }
}
