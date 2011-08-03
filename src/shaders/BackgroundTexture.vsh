uniform mat4 u_ortho;     // model-view-projection matrix
uniform vec4 u_null1;

attribute vec4 a_position;
attribute vec4 a_texCoord;

varying vec2 textureCoordinate;

void main()
{
    gl_Position = u_ortho * a_position;
    textureCoordinate = a_texCoord.xy;
}
