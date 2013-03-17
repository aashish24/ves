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
/// \ingroup ves
/// \see vesTransformNode

#ifndef __vesEigen_h
#define __vesEigen_h

#ifdef _WIN32
  #define EIGEN_DISABLE_UNALIGNED_ARRAY_ASSERT
  #define EIGEN_DONT_ALIGN
#endif
#include "Eigen/Dense"

typedef Eigen::Matrix4f vesMatrix4x4f;
typedef Eigen::Matrix3f vesMatrix3x3f;
typedef Eigen::Affine3f vesAffine3f;
typedef Eigen::Vector2f vesVector2f;
typedef Eigen::Vector3f vesVector3f;
typedef Eigen::Vector4f vesVector4f;
typedef Eigen::Vector2d vesVector2d;
typedef Eigen::Vector3d vesVector3d;
typedef Eigen::Vector4d vesVector4d;
typedef Eigen::Matrix<unsigned short, 3, 1> vesVector3us;
typedef Eigen::Matrix<unsigned short, 2, 1> vesVector2us;


vesMatrix4x4f makeScaleMatrix4x4(float x, float y, float z);
vesMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z);
vesMatrix4x4f makeTranslationMatrix4x4(const vesVector3f& trans);
vesMatrix4x4f makeTransposeMatrix4x4(const vesMatrix4x4f& matrix);
vesMatrix4x4f makeInverseMatrix4x4(const vesMatrix4x4f& matrix);

vesMatrix4x4f vesOrtho(float left,
                       float right,
                       float bottom,
                       float top,
                       float znear,
                       float zfar);

vesMatrix4x4f vesFrustum(float left,
                         float right,
                         float bottom,
                         float top,
                         float znear,
                         float zfar);

vesMatrix3x3f makeNormalMatrix3x3f(const vesMatrix4x4f& matrix);
vesMatrix4x4f makeNormalizedMatrix4x4(const vesMatrix4x4f& matrix);
vesVector3f transformPoint3f(const vesMatrix4x4f& matrix, const vesVector3f& vec);
float deg2Rad(float degree);
vesMatrix4x4f vesLookAt(const vesVector3f& position,
                        const vesVector3f& focalPoint,
                        const vesVector3f& viewUp);
vesMatrix4x4f vesPerspective(float fov, float aspectRatio, float znear,
                             float zfar);

#endif
