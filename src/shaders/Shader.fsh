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

varying lowp vec4  varVertexColor;
varying lowp vec4  varDiffuseColor;
varying lowp vec4  varAmbientColor;
varying lowp float varOpacity;

varying highp vec4 varPosition;
varying highp vec3 varNormal;
varying highp vec3 varLightDirection;

void main()
{  
  lowp float nDotL;
  lowp float nDotH;
  
  // Default to metallic look and feel.
  lowp float specularShininess = 128.0;
  lowp vec4  specularColor     = vec4(0.8, 0.8, 0.8, 0.0);
  
  lowp vec4 color = varAmbientColor;   
  
  highp vec3 n = normalize(varNormal);
    
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
    
  gl_FragColor = vec4(color.xyz, varOpacity);
}
