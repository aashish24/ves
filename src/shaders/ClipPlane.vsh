
// Uniforms.
uniform bool    hasVertexColors;
uniform mediump vec3 lightDirection;
uniform highp mat4   modelViewMatrix;
uniform mediump mat3 normalMatrix;
uniform lowp int     primitiveType;
uniform highp mat4   projectionMatrix;
uniform highp vec4   clipPlaneEquation;

// Vertex attributes.
attribute highp vec3 vertexPosition;
attribute mediump vec3 vertexNormal;
attribute lowp vec4 vertexColor;

// Varying attributes.
varying lowp vec4  varColor;

varying highp float clipDistance;

void main()
{
  // Default ambient color for now.
  lowp vec4 ambientColor = vec4(0.01, 0.01, 0.01, 0.0);
  lowp vec4 diffuseColor = vec4(0.1, 0.1, 0.1, 0.0);

  if (hasVertexColors) {
    varColor = vertexColor + diffuseColor;
  }
  else {
    varColor = vertexColor + ambientColor;
  }

  // Save position for shading later.
  highp vec4 position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // 3 is line
  if (primitiveType != 3) {
    // Transform vertex normal into eye space.
    mediump vec3 normal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    mediump vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.650));

    lowp float nDotL = max(dot(normal, lightDirection), 0.0);

    // Do backface lighting too.
    nDotL = max(dot(-normal, lightDirection), nDotL);

    varColor = vec4(varColor.xyz * nDotL, varColor.w);
  }

  clipDistance = dot(vertexPosition.xyz, clipPlaneEquation.xyz) + clipPlaneEquation.w;

  // GLSL still requires this.
  gl_Position = position;
}
