//
//  vtkGMTL.h
//  kiwi
//
//  Created by kitware on 5/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkGMTL_h
#define __vtkGMTL_h

#include "gmtl/Matrix.h"
#include "gmtl/MatrixOps.h"
#include "gmtl/Point.h"

typedef gmtl::Matrix<float,4,4> vtkMatrix4x4f;
typedef gmtl::Matrix<float,3,3> vtkMatrix3x3f;
typedef gmtl::Vec3f vtkVector3f;
typedef gmtl::Vec4f vtkVector4f;
typedef gmtl::Point3f vtkPoint3f;
typedef gmtl::Vec<unsigned short,3> vtkVector3us;

//class vtkMatrix4x4f : public Matrix4x4f
//{
//public:
//  vtkMatrix4x4f();
//  vtkMatrix4x4f(const vtkMatrix4x4f& copy);
//  ~vtkMatrix4x4f();
//  void Set(vtkMatrix4x4f mat)
//  { *this = mat;}
//  vtkMatrix4x4f translate(vtkVector3f vec);
//  vtkMatrix4x4f rotate(float angle, float x, float y, float z);
//  vtkMatrix4x4f scale(float scale);
//  vtkMatrix4x4f& operator= (const vtkMatrix4x4f &rhs);
//};

vtkMatrix4x4f makeScaleMatrix4x4(float scale);
vtkMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z);
vtkMatrix4x4f makeTranslationMatrix4x4(vtkVector3f trans);
vtkVector3f makeTranslationVector3f(vtkMatrix4x4f matrix);
vtkMatrix4x4f makeTransposeMatrix4x4(vtkMatrix4x4f matrix);

vtkMatrix4x4f makeOrthoMatrix4x4(float left, 
                                 float right, 
                                 float bottom, 
                                 float top, 
                                 float near, 
                                 float far);

vtkMatrix4x4f makePerspectiveMatrix4x4(float left, 
                                      float right, 
                                      float bottom, 
                                      float top, 
                                      float near, 
                                       float far);
vtkMatrix3x3f makeNormalMatrix3x3f(vtkMatrix4x4f matrix);
vtkMatrix4x4f makeNormalizedMatrix4x4(vtkMatrix4x4f matrix);
vtkPoint3f transformVector4f(vtkMatrix4x4f matrix, vtkPoint3f vec);
#endif