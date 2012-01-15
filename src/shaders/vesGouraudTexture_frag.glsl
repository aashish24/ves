varying mediump vec2 textureCoordinate;

//varying mediump vec4 varColor;
varying highp float nDotL;

uniform highp sampler2D image;

void main()
{

  //highp vec4 diffuse = vec4(0.1, 0.1, 0.1, 1.0)
  //gl_FragColor = (texture2D(image, textureCoordinate) + diffuse) * nDotL;

  highp vec4 color = texture2D(image, textureCoordinate) * nDotL;
  gl_FragColor = vec4(color.xyz, 1.0);
}
