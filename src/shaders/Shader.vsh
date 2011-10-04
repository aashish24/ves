
// Uniforms.
uniform mat4   modelViewMatrix;
uniform mat4   projectionMatrix;
uniform mat3   normalMatrix;
uniform vec3   lightDirection;

// Vertex attributes.
attribute vec4 vertexPosition;
attribute vec3 vertexNormal;

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
  varDiffuseColor = vec4(0.1, 0.1, 0.1, 0.0);

  // Save position for shading later.
  varPosition = projectionMatrix * modelViewMatrix * vertexPosition;

  // Transform vertex normal into eye space.
  varNormal = normalize(normalMatrix * vertexNormal);

  // Save light direction (direction light for now)
  varLightDirection = normalize(vec3(0.0, 0.0, 100.0));

  lowp float nDotL = max(dot(varNormal, varLightDirection), 0.0);

  varDiffuseColor = (varDiffuseColor) * nDotL;

  // GLSL still requires this.
  gl_Position = varPosition;
}
