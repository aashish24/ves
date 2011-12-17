uniform highp mat4 modelViewMatrix;
uniform highp mat4 projectionMatrix;
uniform mediump mat3 normalMatrix;

attribute highp vec4 vertexPosition;
attribute mediump vec3 vertexNormal;
attribute mediump vec4 vertexTextureCoordinate;

varying mediump vec2 textureCoordinate;
//varying mediump vec4 varColor;
varying highp float nDotL;

void main()
{



  // Default ambient color for now.
  //lowp vec4 ambientColor = vec4(0.01, 0.01, 0.01, 0.0);
  //lowp vec4 diffuseColor = vec4(0.1, 0.1, 0.1, 0.0);
  //varColor = vec4(1.0, 1.0, 1.0, 0.0);;


  lowp vec3 normal = normalize(normalMatrix * vertexNormal);
  lowp vec3 lightDirection = normalize(vec3(0.0, 0.0, 0.650));

  //lowp float nDotL = max(dot(normal, lightDirection), 0.0);
  //varColor = vec4(varColor.xyz * nDotL, varColor.w);

  nDotL = max(dot(normal, lightDirection), 0.0);


  gl_Position = projectionMatrix * modelViewMatrix * vertexPosition;

  textureCoordinate = vertexTextureCoordinate.xy;
}
