varying mediump vec2 textureCoordinate;

uniform highp sampler2D image;

void main()
{
    gl_FragColor = texture2D(image, textureCoordinate);
}
