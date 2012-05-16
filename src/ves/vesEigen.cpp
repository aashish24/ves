/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

#include "vesEigen.h"

#include "Eigen/Geometry"

vesMatrix4x4f makeScaleMatrix4x4(float x, float y ,float z)
{
  Eigen::Affine3f t(Eigen::Scaling(x, y, z));
  return t.matrix();
}

vesMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z)
{
  Eigen::Affine3f t(Eigen::AngleAxisf(angle, Eigen::Vector3f(x, y, z)));
  return t.matrix();
}

vesMatrix4x4f makeTranslationMatrix4x4(vesVector3f trans)
{
  Eigen::Transform<float, 3, Eigen::Affine> t;
  t = Eigen::Translation3f(trans);
  return t.matrix();
}

vesMatrix4x4f makeTransposeMatrix4x4(vesMatrix4x4f matrix)
{
  return matrix.transpose();
}

vesMatrix4x4f makeInverseMatrix4x4(vesMatrix4x4f matrix)
{
  return matrix.inverse();
}

vesMatrix4x4f vesOrtho(float left,
                       float right,
                       float bottom,
                       float top,
                       float near,
                       float far)
{
  Eigen::Matrix4f m;
  float a = 2.0f / (right - left);
  float b = 2.0f / (top - bottom);
  float c = -2.0f / (far - near);
  float tx = -(right + left) / (right - left);
  float ty = -(top + bottom) / (top - bottom);
  float tz = -(far + near) / (far - near);
  m << a,  0,  0,  tx,
       0,  b,  0,  ty,
       0,  0,  c,  tz,
       0,  0,  0,  1;
  return m;
}

vesMatrix4x4f vesFrustum(float left,
                         float right,
                         float bottom,
                         float top,
                         float near,
                         float far)
{
  Eigen::Matrix4f m;
  float a = 2 * near / (right - left);
  float b = 2 * near / (top - bottom);
  float c = (right + left) / (right - left);
  float d = (top + bottom) / (top - bottom);
  float e = - (far + near) / (far - near);
  float f = -2 * far * near / (far - near);
  m << a,  0,  c,  0,
       0,  b,  d,  0,
       0,  0,  e,  f,
       0,  0, -1,  0;
  return m;
}

vesMatrix3x3f makeNormalMatrix3x3f(vesMatrix4x4f matrix)
{
  Eigen::Matrix3f m(matrix.topLeftCorner<3, 3>());
  return m;
}

vesMatrix4x4f makeNormalizedMatrix4x4(vesMatrix4x4f matrix)
{
  vesMatrix4x4f mat = matrix;
  float length = sqrt(mat(0, 0) * mat(0, 0) + mat(0, 1) * mat(0, 1)
                      + mat(0, 2) * mat(0, 2));
  mat(0, 0) = mat(0, 0) / length;
  mat(0, 1) = mat(0, 1) / length;
  mat(0, 2) = mat(0, 2) / length;

  length = sqrt(mat(1, 0) * mat(1, 0) + mat(1, 1) * mat(1, 1)
                + mat(1, 2) * mat(1, 2));
  mat(1, 0) = mat(1, 0) / length;
  mat(1, 1) = mat(1, 1) / length;
  mat(1, 2) = mat(1, 2) / length;

  length = sqrt(mat(2, 0) * mat(2, 0) + mat(2, 1) * mat(2, 1)
                + mat(2, 2) * mat(2, 2));
  mat(2, 0) = mat(2, 0) / length;
  mat(2, 1) = mat(2, 1) / length;
  mat(2, 2) = mat(2, 2) / length;
  return mat;
}

vesVector3f transformPoint3f(vesMatrix4x4f matrix, vesVector3f vec)
{
  return Eigen::Affine3f(matrix) * vec;
}

// Wouldn't this be better placed in the header so it can be inlined?
float deg2Rad(float degree)
{
  return 0.0174532925 * degree;
}

vesMatrix4x4f vesLookAt(vesVector3f position,
                        vesVector3f focalPoint,
                        vesVector3f viewUp)
{
  Eigen::Matrix4f matrix;
  matrix.setIdentity();

  Eigen::Vector3f viewSideways, orthoViewUp, viewPlaneNormal;
  viewPlaneNormal = position - focalPoint;
  viewPlaneNormal.normalize();
  viewSideways = viewUp.cross(viewPlaneNormal);
  viewSideways.normalize();
  orthoViewUp = viewPlaneNormal.cross(viewSideways);

  matrix(0, 0) = viewSideways[0];
  matrix(0, 1) = viewSideways[1];
  matrix(0, 2) = viewSideways[2];

  matrix(1, 0) = orthoViewUp[0];
  matrix(1, 1) = orthoViewUp[1];
  matrix(1, 2) = orthoViewUp[2];

  matrix(2, 0) = viewPlaneNormal[0];
  matrix(2, 1) = viewPlaneNormal[1];
  matrix(2, 2) = viewPlaneNormal[2];

  Eigen::Vector4f delta;
  delta[0] = -position[0];
  delta[1] = -position[1];
  delta[2] = -position[2];
  delta[3] = 0.0; // yes, this should be zero, not one

  Eigen::Vector4f d2;
  d2 = matrix * delta;

  matrix(0, 3) = d2[0];
  matrix(1, 3) = d2[1];
  matrix(2, 3) = d2[2];

  return matrix;
}

vesMatrix4x4f vesPerspective(float fovY,float aspect, float zNear,float zFar)
{
  float width, height;
  height = tan( deg2Rad(fovY)/2 ) * zNear;
  width = height * aspect;
  return vesFrustum(-width, width, -height, height, zNear, zFar);
}
