uniform mat4 modelViewProjectionMatrix;

attribute vec4 vertexPosition;
attribute vec4 vertexTextureCoordinate;

varying vec2 textureCoordinate;

void main()
{
    gl_Position = modelViewProjectionMatrix * vertexPosition;
    textureCoordinate = vertexTextureCoordinate.xy;
}
