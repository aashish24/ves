uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

attribute vec4 vertexPosition;
attribute vec4 vertexTextureCoordinate;

varying vec2 textureCoordinate;

void main()

{
    gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;

    textureCoordinate = vertexTextureCoordinate.xy;
}
