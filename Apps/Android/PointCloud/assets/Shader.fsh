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
//precision mediump float;

varying lowp vec4 v_texcoord;
varying lowp vec2 v_tcoord;
//varying lowp vec2 screenPos;
uniform sampler2D s_texture;

void main()
{
    //if (distance(gl_FragCoord.xy, screenPos) > 5.0) discard;
    //gl_FragColor = v_texcoord;
    gl_FragColor = texture2D(s_texture, v_tcoord);
}
