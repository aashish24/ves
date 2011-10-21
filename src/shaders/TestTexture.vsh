uniform highp mat4 modelViewMatrix;
uniform highp mat4 projectionMatrix;

attribute highp vec4 vertexPosition;
attribute mediump vec4 vertexTextureCoordinate;

varying mediump vec2 textureCoordinate;

void main()
{
  gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;

  textureCoordinate = vec2(vertexTextureCoordinate.x, 0.0);
}
