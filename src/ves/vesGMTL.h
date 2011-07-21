//
//  vesGMTL.h
//  kiwi
//
//  Created by kitware on 5/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vesGMTL_h
#define __vesGMTL_h

#include "gmtl/AxisAngle.h"
#include "gmtl/Generate.h"
#include "gmtl/Matrix.h"
#include "gmtl/MatrixOps.h"
#include "gmtl/Point.h"

typedef gmtl::Matrix<float,4,4> vesMatrix4x4f;
typedef gmtl::Matrix<float,3,3> vesMatrix3x3f;
typedef gmtl::Vec3f vesVector3f;
typedef gmtl::Vec4f vesVector4f;
typedef gmtl::Point3f vtkPoint3f;
typedef gmtl::Vec<unsigned short,3> vesVector3us;
typedef gmtl::Vec<unsigned short, 2> vesVector2us;

//class vesMatrix4x4f : public Matrix4x4f
//{
//public:
//  vesMatrix4x4f();
//  vesMatrix4x4f(const vesMatrix4x4f& copy);
//  ~vesMatrix4x4f();
//  void Set(vesMatrix4x4f mat)
//  { *this = mat;}
//  vesMatrix4x4f translate(vesVector3f vec);
//  vesMatrix4x4f rotate(float angle, float x, float y, float z);
//  vesMatrix4x4f scale(float scale);
//  vesMatrix4x4f& operator= (const vesMatrix4x4f &rhs);
//};

vesMatrix4x4f makeScaleMatrix4x4(float x, float y, float z);
vesMatrix4x4f makeRotationMatrix4x4(float angle, float x, float y, float z);
vesMatrix4x4f makeTranslationMatrix4x4(vesVector3f trans);
vesVector3f makeTranslationVector3f(vesMatrix4x4f matrix);
vesMatrix4x4f makeTransposeMatrix4x4(vesMatrix4x4f matrix);
vesMatrix4x4f makeInverseMatrix4x4(vesMatrix4x4f matrix);

vesMatrix4x4f makeOrthoMatrix4x4(float left, 
                                 float right, 
                                 float bottom, 
                                 float top, 
                                 float near, 
                                 float far);

vesMatrix4x4f makePerspectiveMatrix4x4(float left, 
                                      float right, 
                                      float bottom, 
                                      float top, 
                                      float near, 
                                       float far);
vesMatrix3x3f makeNormalMatrix3x3f(vesMatrix4x4f matrix);
vesMatrix4x4f makeNormalizedMatrix4x4(vesMatrix4x4f matrix);
vtkPoint3f transformPoint3f(vesMatrix4x4f matrix, vtkPoint3f vec);
#endif
