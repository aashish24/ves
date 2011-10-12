
// Uniforms.
uniform bool   hasVertexColors;
uniform vec3   lightDirection;
uniform mat4   modelViewMatrix;
uniform mat3   normalMatrix;
uniform int    primitiveType;
uniform mat4   projectionMatrix;

// Vertex attributes.
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec3 vertexColor;

// Varying attributes.
varying lowp vec4  varColor;

void main()
{
  // Default ambient color for now.
  vec4 ambientColor = vec4(0.01, 0.01, 0.01, 0.0);

  // Default diffuse color for now.
  if (hasVertexColors) {
    varColor = vec4(0.1, 0.1, 0.1, 1.0) + vec4(vertexColor, 1.0);
  }
  else {
    varColor = vec4(0.9, 0.9, 0.9, 1.0) + ambientColor;
  }

  // Save position for shading later.
  vec4 position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // 3 is line
  if (primitiveType != 3) {
    // Transform vertex normal into eye space.
    vec3 normal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.650));

    lowp float nDotL = max(dot(normal, lightDirection), 0.0);

    varColor = (varColor) * nDotL;
  }

  // GLSL still requires this.
  gl_Position = position;
}
