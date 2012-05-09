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

#include <ves/vesMath.h>

#include <iostream>
#include <cmath>
#include <string>

using std::cout;
using std::endl;

float* getMatrixData(vesMatrix4x4f &matrix)
{
#ifdef EIGEN_CORE_H
  float *p = matrix.data();
#else
  float *p = matrix.mData;
#endif
  return p;
}

float* getMatrixData(vesMatrix3x3f &matrix)
{
#ifdef EIGEN_CORE_H
  float *p = matrix.data();
#else
  float *p = matrix.mData;
#endif
  return p;
}

void printMatrix(vesMatrix4x4f &matrix)
{
  float *p = getMatrixData(matrix);
  for (int i = 0; i < 16; ++i) {
    if (i % 4 == 0)
      cout << endl;
    cout << p[i] << " ";
  }
}

void printMatrix(vesMatrix3x3f &matrix)
{
  float *p = getMatrixData(matrix);
  for (int i = 0; i < 9; ++i) {
    if (i % 3 == 0)
      cout << endl;
    cout << p[i] << " ";
    }
}

bool fuzzyCompare(float a, float b)
{
  if (std::fabs(a - b) < 0.001)
    return true;
  else
    return false;
}

bool compareMatrices(vesMatrix4x4f &matrix, float other[16], std::string name)
{
  float *data = getMatrixData(matrix);
  for (int i = 0; i < 16; ++i) {
    if (!fuzzyCompare(data[i], other[i])) {
      cout << name << " matrix DOES NOT match!";
      cout << "i:" << i << " -> " << data[i] << " != " << other[i] << endl;
      printMatrix(matrix);
      cout << endl;
      return false;
    }
  }
  cout << name << " matrix: ";
  printMatrix(matrix);
  cout << endl;
  return true;
}

bool compareMatrices(vesMatrix3x3f &matrix, float other[9], std::string name)
{
  float *data = getMatrixData(matrix);
  for (int i = 0; i < 9; ++i) {
    if (!fuzzyCompare(data[i], other[i])) {
      cout << name << " matrix does not match!";
      printMatrix(matrix);
      cout << endl;
      return false;
    }
  }
  cout << name << " matrix: ";
  printMatrix(matrix);
  cout << endl;
  return true;
}

int main(int, char *[])
{
  // Be optimistic, but prepared to fail...
  bool success = true;

  // Test the scale matrix.
  vesMatrix4x4f scale = makeScaleMatrix4x4(1.0, 2.0, 3.0);
  float scaleArray[] = { 1, 0, 0, 0,
                         0, 2, 0, 0,
                         0, 0, 3, 0,
                         0, 0, 0, 1 };
  if (!compareMatrices(scale, scaleArray, "Scale"))
    success = false;

  // Test the rotation matrix.
  vesMatrix4x4f rot = makeRotationMatrix4x4(66.6, 1.0, 2.0, 3.0);
  float rotArray[] = { 1,       1.86104, 6.60287, 0,
                       5.37917, 6.43016, 10.274,  0,
                       4.25745, 11.4467, 15.4804, 0,
                       0,       0,       0,       1};
  if (!compareMatrices(rot, rotArray, "Rotation"))
    success = false;

  // Test the translation matrix.
  vesMatrix4x4f trans = makeTranslationMatrix4x4(vesVector3f(1, 2, 3));
  float tranArray[] = { 1, 0, 0, 0,
                        0, 1, 0, 0,
                        0, 0, 1, 0,
                        1, 2, 3, 1 };
  if (!compareMatrices(trans, tranArray, "Translation"))
    success = false;

  // Test out transpose matrix.
  vesMatrix4x4f transpose = makeTransposeMatrix4x4(trans);
  float transposeArray[] = { 1, 0, 0, 1,
                             0, 1, 0, 2,
                             0, 0, 1, 3,
                             0, 0, 0, 1 };
  if (!compareMatrices(transpose, transposeArray, "Transpose"))
    success = false;

  // Test out inverse matrix.
  vesMatrix4x4f inverse = makeInverseMatrix4x4(trans);
  float inverseArray[] = {  1, -0,  0, -0,
                           -0,  1, -0,  0,
                            0, -0,  1, -0,
                           -1, -2, -3,  1};
  if (!compareMatrices(inverse, inverseArray, "Inverse"))
    success = false;

  // Test out the ortho matrix code.
  vesMatrix4x4f ortho = vesOrtho(0, 2, 0, 3, -0.5, 1.7);
  float orthoArray[] = { 1,  0,        0,        0,
                         0,  0.666667, 0,        0,
                         0,  0,       -0.909091, 0,
                        -1, -1,       -0.545455, 1 };
  if (!compareMatrices(ortho, orthoArray, "Orthographic"))
    success = false;

  // Test out the frustrum matrix code.
  vesMatrix4x4f frustrum = vesFrustum(0, 2, 0, 3, -0.5, 1.7);
  float frustrumArray[] = { -0.5, 0,        0,         0,
                             0,  -0.333333, 0,         0,
                             1,   1,       -0.545455, -1,
                             0,   0,        0.772727,  0 };
  if (!compareMatrices(frustrum, frustrumArray, "Frustrum"))
    success = false;

  // Test the normal matrix code.
  vesMatrix3x3f normal = makeNormalMatrix3x3f(ortho);
  float normalArray[] = { 1, 0,        0,
                          0, 0.666667, 0,
                          0, 0,       -0.909091 };
  if (!compareMatrices(normal, normalArray, "Normal"))
    success = false;

  // Test out the matrix normalization.
  vesMatrix4x4f normRot = makeNormalizedMatrix4x4(rot);
  float normRotArray[] = { 0.144245, 0.140346, 0.334867, 0,
                           0.775921, 0.484916, 0.521047, 0,
                           0.614117, 0.863226, 0.785095, 0,
                           0,        0,        0,        1 };
  if (!compareMatrices(normRot, normRotArray, "Normalized Rotation"))
    success = false;

  // Point translation.
  vesVector3f point = transformPoint3f(trans, vesVector3f(1, 3, 5));
  float pointArray[] = { 2, 5, 8 };
  cout << "Point comparison: ";
  for (int i = 0; i < 3; ++i) {
    if (!fuzzyCompare(point[i], pointArray[i])) {
      cout << "Point comparison failed. " << point[i] << "!=" << pointArray[i]
           << endl;
      success = false;
    }
    cout << point[i] << ":" << pointArray[i] << ", ";
  }
  cout << endl;

  // Test out deg2rad...
  if (!fuzzyCompare(deg2Rad(69), 1.20428)) {
    cout << "deg2Rad(69) = " << deg2Rad(69) << endl;
    success = false;
  }

  // Test vesLookAt...
  vesMatrix4x4f lookAt = vesLookAt(vesVector3f(1, 2, 3),
                                   vesVector3f(5, 5, 5),
                                   vesVector3f(1, 0, 0));
  float lookAtArray[] = {  0,        0.669534, -0.742781, 0,
                           0.5547,  -0.618031, -0.557086, 0,
                          -0.83205, -0.412021, -0.371391, 0,
                           1.38675,  1.80259,   2.97113,  1 };
  if (!compareMatrices(lookAt, lookAtArray, "Look At"))
    success = false;

  // Test vesPerspective...
  vesMatrix4x4f perspective = vesPerspective(0.9, 4.33, 0.1, 9.9);
  float perspectiveArray[] = { 29.4045, 0,       0,        0,
                               0,       127.321, 0,        0,
                               0,       0,      -1.02041, -1,
                               0,       0,      -0.202041, 0 };
  if (!compareMatrices(perspective, perspectiveArray, "Perspective"))
    success = false;

  cout << "Success = " << success << endl;

  return success ? 0 : 1;
}
