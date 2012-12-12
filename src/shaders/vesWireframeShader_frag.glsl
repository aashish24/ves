/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
/// \file vesWireframeShader_frag.glsl
///
/// Implementation based on demo code from http://www2.imm.dtu.dk/~janba/Wireframe/
/// \ingroup shaders

uniform lowp float lineWidth;

varying lowp vec4 varColor;
varying highp vec3 dist;

void main()
{

  // Undo perspective correction.
  highp vec3 dist_vec = dist * gl_FragCoord.w;

  // Compute the shortest distance to the edge
  highp float d = min(dist_vec[0], min(dist_vec[1], dist_vec[2]));



#ifdef SURFACE_WITH_EDGES

  lowp vec4 fillColor = varColor;
  lowp vec4 edgeColor = fillColor - vec4(0.4, 0.4, 0.4, 0.0);
  lowp float a = 2.0;
  lowp float myLineWidth = (lineWidth-1.0)*0.5;
  d = max(d - myLineWidth, 0.0);

#else

  lowp float isFront = float(gl_FrontFacing);

  lowp float myLineWidth = isFront*(lineWidth-1.0)*0.5;

  if (d > myLineWidth + 1.0) discard;

  lowp vec4 edgeColor = varColor*isFront + vec4(0.3, 0.3, 0.3, varColor.a)*(1.0-isFront);
  lowp vec4 fillColor = vec4(edgeColor.rgb, 0.0);
  lowp float a = 2.0;
  d = max(d - myLineWidth, 0.0);

#endif

  // Compute line intensity and then fragment color
  highp float I = exp2(-a*d*d);
  gl_FragColor.xyzw = I*edgeColor + (1.0 - I)*fillColor;
}
