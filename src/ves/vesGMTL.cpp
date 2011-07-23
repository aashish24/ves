#include "vesGMTL.h"
#include "gmtl/Generate.h"
#include "gmtl/AxisAngle.h"

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeScaleMatrix4x4(float x, float y ,float z)
{
  return gmtl::makeScale<vesMatrix4x4f>(vesVector3f(x,y,z));
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z)
{
  return gmtl::makeRot<vesMatrix4x4f>(gmtl::AxisAngle<float>(angle,x,y,z));
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeTranslationMatrix4x4(vesVector3f trans)
{
  return gmtl::makeTrans<vesMatrix4x4f>(trans);
}

// -------------------------------------------------------------------------fun
vesVector3f makeTranslationVector3f(vesMatrix4x4f matrix)
{
  return gmtl::makeTrans<vesVector3f>(matrix);
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeTransposeMatrix4x4(vesMatrix4x4f matrix)
{
  vesMatrix4x4f mat = matrix;
  gmtl::transpose(mat);
  return mat;
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeInverseMatrix4x4(vesMatrix4x4f matrix)
{
  vesMatrix4x4f mat = matrix;
  gmtl::invert(mat);
  return mat;
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f vesOrtho(float left,
                       float right,
                       float bottom,
                       float top,
                       float near,
                       float far)
{
  vesMatrix4x4f mat;
  float a = 2.0f / (right - left);
  float b = 2.0f / (top - bottom);
  float c = -2.0f / (far - near);
  float tx = (right + left) / (right - left);
  float ty = (top + bottom) / (top - bottom);
  float tz = (far + near) / (far - near);
  mat[0][0] = a; mat[0][1] = 0; mat[0][2] = 0; mat[0][3] = tx;
  mat[1][0] = 0; mat[1][1] = b; mat[1][2] = 0; mat[1][3] = ty;
  mat[2][0] = 0; mat[2][1] = 0; mat[2][2] = c; mat[2][3] = tz;
  mat[3][0] = 0; mat[3][1] = 0; mat[3][2] = 0; mat[3][3] = 1;
  return mat;
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f vesFrustum(float left,
			 float right,
			 float bottom,
			 float top,
			 float near,
			 float far)
{
  vesMatrix4x4f mat;
  float a = 2 * near / (right - left);
  float b = 2 * near / (top - bottom);
  float c = (right + left) / (right - left);
  float d = (top + bottom) / (top - bottom);
  float e = - (far + near) / (far - near);
  float f = -2 * far * near / (far - near);
  mat[0][ 0] = a; mat[0][ 1] = 0; mat[0][ 2] = c; mat[0][ 3] = 0;
  mat[1][ 0] = 0; mat[1][ 1] = b; mat[1][ 2] = d; mat[1][ 3] = 0;
  mat[2][ 0] = 0; mat[2][ 1] = 0; mat[2][ 2] = e; mat[2][ 3] = f;
  mat[3][ 0] = 0; mat[3][ 1] = 0; mat[3][ 2] = -1; mat[3][ 3] = 0;
  return mat;
}

// -------------------------------------------------------------------------fun
vesMatrix3x3f makeNormalMatrix3x3f(vesMatrix4x4f matrix)
{
  vesMatrix3x3f mat;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      mat[i][j] = matrix[i][j];
    }
  }
  return mat;
}

// -------------------------------------------------------------------------fun
vesMatrix4x4f makeNormalizedMatrix4x4(vesMatrix4x4f matrix)
{
  vesMatrix4x4f mat = matrix;
  float length = sqrt(mat[0][0]*mat[0][0] + mat[0][1]*mat[0][1]
                      + mat[0][2]*mat[0][2]);
  mat[0][0] = mat[0][0] / length;
  mat[0][1] = mat[0][1] / length;
  mat[0][2] = mat[0][2] / length;

  length = sqrt(mat[1][0]*mat[1][0] + mat[1][1]*mat[1][1]
                + mat[1][2]*mat[1][2]);
  mat[1][0] = mat[1][0] / length;
  mat[1][1] = mat[1][1] / length;
  mat[1][2] = mat[1][2] / length;

  length = sqrt(mat[2][0]*mat[2][0] + mat[2][1]*mat[2][1]
                + mat[2][2]*mat[2][2]);
  mat[2][0] = mat[2][0] / length;
  mat[2][1] = mat[2][1] / length;
  mat[2][2] = mat[2][2] / length;
  return mat;
}

// -------------------------------------------------------------------------fun
vtkPoint3f transformPoint3f(vesMatrix4x4f matrix, vtkPoint3f vec)
{
  vtkPoint3f ret;
  gmtl::xform(ret,matrix,vec);
  return ret;
}

float deg2Rad(float degree)
{
  return gmtl::Math::deg2Rad(degree);
}

void pv(std::string name, vesVector3f v)
  {
  std::cerr << name << ": (" << v[0] << "," << v[1] << "," << v[2] << ")" << std::endl;
  }

// ----------------------------------------------------------------------------
vesMatrix4x4f vesLookAt( vesVector3f position,
			 vesVector3f focalPoint,
			 vesVector3f viewUp)
{
  pv("position", position);
  pv("focalPoint", focalPoint);
  pv("viewUp", viewUp);
  
  //double matrix[4][4];
  //vtkMatrix4x4::Identity(*matrix);
  vesMatrix4x4f matrix;
  gmtl::identity(matrix);

  // the view directions correspond to the rows of the rotation matrix,
  // so we'll make the connection explicit
  //double *viewSideways =    matrix[0];
  //double *orthoViewUp =     matrix[1];
  //double *viewPlaneNormal = matrix[2];
  gmtl::Vec3f viewSideways, orthoViewUp, viewPlaneNormal;

  // set the view plane normal from the view vector
  //viewPlaneNormal[0] = position[0] - focalPoint[0];
  //viewPlaneNormal[1] = position[1] - focalPoint[1];
  //viewPlaneNormal[2] = position[2] - focalPoint[2];
  viewPlaneNormal = position - focalPoint;
  
  //vtkMath::Normalize(viewPlaneNormal);
  gmtl::normalize(viewPlaneNormal);

  // orthogonalize viewUp and compute viewSideways
  //vtkMath::Cross(viewUp,viewPlaneNormal,viewSideways);
  //vtkMath::Normalize(viewSideways);
  //vtkMath::Cross(viewPlaneNormal,viewSideways,orthoViewUp);
  viewSideways = gmtl::makeCross(viewUp,viewPlaneNormal);
  gmtl::normalize(viewSideways);
  orthoViewUp = gmtl::makeCross(viewPlaneNormal,viewSideways);

  matrix[0][0] = viewSideways[0];
  matrix[0][1] = viewSideways[1];
  matrix[0][2] = viewSideways[2];

  matrix[1][0] = orthoViewUp[0];
  matrix[1][1] = orthoViewUp[1];
  matrix[1][2] = orthoViewUp[2];

  matrix[2][0] = viewPlaneNormal[0];
  matrix[2][1] = viewPlaneNormal[1];
  matrix[2][2] = viewPlaneNormal[2];

  // translate by the vector from the position to the origin
  //double delta[4];
  //delta[0] = -position[0];
  //delta[1] = -position[1];
  //delta[2] = -position[2];
  //delta[3] = 0.0; // yes, this should be zero, not one
  vesVector4f delta;
  delta[0] = -position[0];
  delta[1] = -position[1];
  delta[2] = -position[2];
  delta[3] = 0.0; // yes, this should be zero, not one

  //vtkMatrix4x4::MultiplyPoint(*matrix,delta,delta);
  gmtl::xform(delta, matrix,delta);

  matrix[0][3] = delta[0];
  matrix[1][3] = delta[1];
  matrix[2][3] = delta[2];

  return matrix;
}

// ----------------------------------------------------------------------public
vesMatrix4x4f vesPerspective(float fovY,float aspect, float zNear,float zFar)
{
  const float pi = 3.14195926535897932384626433832795;
  float width,height;
  height = tan( deg2Rad(fovY)/2 ) * zNear;
  width = height * aspect;
  return vesFrustum( -width, width, -height, height, zNear, zFar );
}
