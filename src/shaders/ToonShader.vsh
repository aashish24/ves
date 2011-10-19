
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

// Varying attributes.
varying mediump vec4 varPosition;
varying mediump vec3 varNormal;
varying mediump vec3 varLightDirection;

void main()
{
  varPosition = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // 3 is line
  if (primitiveType != 3) {
    // Transform vertex normal into eye space.
    varNormal = normalize(normalMatrix * vertexNormal);

    // Save light direction (direction light for now)
    varLightDirection = normalize(vec3(0.0, 0.0, 0.650));
  }

  // GLSL still requires this.
  gl_Position = varPosition;
}
