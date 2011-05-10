//
//  vtkGMTL.cpp
//  kiwi
//
//  Created by kitware on 5/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkGMTL.h"
#include "gmtl/Generate.h"
#include "gmtl/AxisAngle.h"

//vtkMatrix4x4f::vtkMatrix4x4f()
//{
//  
//}
//
//vtkMatrix4x4f::vtkMatrix4x4f(const vtkMatrix4x4f& copy)
//{
//  *this = copy;
//}
//
//vtkMatrix4x4f vtkMatrix4x4f::translate(vtkVector3f vec)
//{
//  vtkMatrix4x4f temp = *this;
//  return temp * gmtl::makeTrans<vtkMatrix4x4f >(vec);
//}
//
//vtkMatrix4x4f vtkMatrix4x4f::rotate(float angle, float x, float y, float z)
//{
//  return (*this) * gmtl::makeRot<vtkMatrix4x4f>(gmtl::AxisAngle<float>(angle,x,y,z));
//  
//}
//
//vtkMatrix4x4f vtkMatrix4x4f::scale(float scale)
//{
//  return (*this) * gmtl::makeScale<vtkMatrix4x4f>(scale);
//}
//
//vtkMatrix4x4f vtkMatrix4x4f::operator= (const vtkMatrix4x4f& rhs)
//{
//  *this = rhs;
//  return *this;
//}

vtkMatrix4x4f makeScaleMatrix4x4(float scale)
{
  return gmtl::makeScale<vtkMatrix4x4f>(scale);
}

vtkMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z)
{
  return gmtl::makeRot<vtkMatrix4x4f>(gmtl::AxisAngle<float>(angle,x,y,z));
}

vtkMatrix4x4f makeTranslationMatrix4x4(vtkVector3f trans)
{
  return gmtl::makeTrans<vtkMatrix4x4f>(trans);
}

vtkVector3f makeTranslationVector3f(vtkMatrix4x4f matrix)
{
  return gmtl::makeTrans<vtkVector3f>(matrix);
}

vtkMatrix4x4f makeTransposeMatrix4x4(vtkMatrix4x4f matrix)
{
  vtkMatrix4x4f mat = matrix;
  gmtl::transpose(mat);
  return mat;
}

vtkMatrix4x4f makeOrthoMatrix4x4(float left, 
                                 float right, 
                                 float bottom, 
                                 float top, 
                                 float near, 
                                 float far)
{
  vtkMatrix4x4f mat;
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

vtkMatrix4x4f makePerspectiveMatrix4x4(float left, 
                                      float right, 
                                      float bottom, 
                                      float top, 
                                      float near, 
                                      float far)
{
  vtkMatrix4x4f mat;
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

vtkMatrix3x3f makeNormalMatrix3x3f(vtkMatrix4x4f matrix)
{
  vtkMatrix3x3f mat;
  for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			mat[i][j] = matrix[i][j];
		}
	}
  return mat;
}

vtkMatrix4x4f makeNormalizedMatrix4x4(vtkMatrix4x4f matrix)
{
  vtkMatrix4x4f mat = matrix;
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

vtkPoint3f transformVector4f(vtkMatrix4x4f matrix, vtkPoint3f vec)
{
  vtkPoint3f ret;
  gmtl::xform(ret,matrix,vec);
  return ret;
}