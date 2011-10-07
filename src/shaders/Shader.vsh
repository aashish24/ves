
// Uniforms.
uniform mat4   modelViewMatrix;
uniform mat4   projectionMatrix;
uniform mat3   normalMatrix;
uniform vec3   lightDirection;

// Vertex attributes.
attribute vec3 vertexPosition;
attribute vec3 vertexNormal;
attribute vec3 vertexColor;

// Varying attributes.
varying lowp vec4  varDiffuseColor;
varying lowp vec4  varAmbientColor;

varying highp vec4 varPosition;
varying highp vec3 varNormal;
varying highp vec3 varLightDirection;

void main()
{
  // Default ambient color for now.
  varAmbientColor = vec4(0.01, 0.01, 0.01, 0.0);

  // Default diffuse color for now.
  varDiffuseColor = vec4(0.2, 0.2, 0.2, 1.0) + vec4(vertexColor, 1.0);

  // Save position for shading later.
  varPosition = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1.0);

  // Transform vertex normal into eye space.
  varNormal = normalize(normalMatrix * vertexNormal);

  // Save light direction (direction light for now)
  varLightDirection = normalize(vec3(0.0, 10.0, 100.0));

  lowp float nDotL = max(dot(varNormal, varLightDirection), 0.0);

  varDiffuseColor = (varDiffuseColor) * nDotL;

  // GLSL still requires this.
  gl_Position = varPosition;
}
