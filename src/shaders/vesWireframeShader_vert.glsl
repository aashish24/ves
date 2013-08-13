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
/// \file vesWireframeShader_vert.glsl
///
/// Implementation based on demo code from http://www2.imm.dtu.dk/~janba/Wireframe/
/// \ingroup shaders

// Uniforms.
uniform mediump vec3 lightDirection;
uniform highp mat4   modelViewMatrix;
uniform mediump mat3 normalMatrix;
uniform lowp int     primitiveType;
uniform lowp int     pointSize;
uniform lowp float   vertexOpacity;
uniform highp mat4 projectionMatrix;
uniform mediump vec2 windowSize;

// Vertex attributes.
attribute highp vec3   vertexPosition;
attribute mediump vec3 vertexNormal;
attribute lowp vec3    vertexColor;

attribute highp vec3 tri_p1;
attribute highp vec3 tri_p2;
//attribute highp vec3 tri_point_indices;
attribute lowp float tri_point_index;


// Varying attributes.
varying lowp vec4 varColor;
varying vec3 dist;

void main()
{

  varColor = vec4(vertexColor, vertexOpacity);

  // 1 is line
  if (primitiveType != 1 && primitiveType != 0) {
    // Transform vertex normal into eye space.
    lowp vec3 normal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    lowp vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.650));

    lowp float nDotL = max(dot(normal, lightDirection), 0.0);

    // Do backface lighting too.
    nDotL = max(dot(-normal, lightDirection), nDotL);

    varColor = vec4(varColor.xyz * nDotL, varColor.w);
  }

  gl_PointSize = float(pointSize);



  highp vec4 p1_3d = vec4(tri_p1, 1.0);
  highp vec4 p2_3d = vec4(tri_p2, 1.0);



  // Compute the vertex position in the usual fashion.
  gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);


  // p0 is the 2D position of the current vertex.
  highp vec2 p0 = gl_Position.xy/gl_Position.w;

  // Project p1 and p2 and compute the vectors v1 = p1-p0
  // and v2 = p2-p0
  highp vec4 p1_3d_ = projectionMatrix * modelViewMatrix * p1_3d;
  highp vec2 v1 = windowSize*(p1_3d_.xy / p1_3d_.w - p0);

  highp vec4 p2_3d_ = projectionMatrix * modelViewMatrix * p2_3d;
  highp vec2 v2 = windowSize*(p2_3d_.xy / p2_3d_.w - p0);

  // Compute 2D area of triangle.
  highp float area2 = abs(v1.x*v2.y - v1.y * v2.x);

  // Compute distance from vertex to line in 2D coords
  highp float h = area2/length(v1-v2);

  // ---
  // The selfIndex variable tells us which of the three vertices
  // we are dealing with. The ugly comparisons would not be needed if
  // selfIndex was an int.

  //dist = vec3(0.0, 0.0, 0.0);
  //dist[tri_point_index] = h;
  if(tri_point_index < 0.1)
    dist = vec3(h,0,0);
  else if(tri_point_index < 1.1)
    dist = vec3(0,h,0);
  else
    dist = vec3(0,0,h);

  // ----
  // Quick fix to defy perspective correction

  dist *= gl_Position.w;
}
