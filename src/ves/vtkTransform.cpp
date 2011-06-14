//
//  vtkTransform.cpp
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkTransform.h"
#include "vtkGMTL.h"

class vtkTransformInternals
{
  vtkMatrix4x4f T,C,R,SR,S,_SR,_C;
public:
  vtkMatrix4x4f Eval()
  {
    return T * C * R * SR * S * _SR * _C;
  }
  
  void SetTranslation(vtkVector3f trans)
  {
    T = makeTranslationMatrix4x4(trans);
  }
  void SetCenter(vtkVector3f center)
  {
    C = makeTranslationMatrix4x4(center);
    _C = makeInverseMatrix4x4(C);
  }
  void SetRotation(vtkVector4f rot)
  {
    R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
  }
  
  void SetScaleOrientation(vtkVector4f sr)
  {
    SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
    _SR = makeInverseMatrix4x4(SR);
  }
  
  void SetScale(vtkVector3f scale)
  {
    S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
  }
};

vtkTransform::vtkTransform()
{
  this->Internals = new vtkTransformInternals;
}
vtkTransform::~vtkTransform()
{
  delete this->Internals;
}

void vtkTransform::Eval()
{
  this->SetInternals();
  this->Internals->Eval();
}

void vtkTransform::SetInternals()
{
  this->Internals->SetTranslation(this->Translation);
  this->Internals->SetCenter(this->Center);
  this->Internals->SetRotation(this->Rotation);
  this->Internals->SetScaleOrientation(this->ScaleOrientation);
  this->Internals->SetScale(this->Scale);
}

void vtkTransform::SetCenter(vtkVector3f center)
{
  this->Center = center;
}

vtkVector3f vtkTransform::GetCenter()
{
  return this->Center;
}

void vtkTransform::SetRotation(vtkVector4f rotation)
{
  this->Rotation = rotation;
}

vtkVector4f vtkTransform::GetRotation()
{
  return this->Rotation;
}

void vtkTransform::SetScale(vtkVector3f scale)
{
  this->Scale = scale;
}

vtkVector3f vtkTransform::GetScale()
{
  return this->Scale;
}

void vtkTransform::SetScaleOrientation(vtkVector4f scaleOrientation)
{
  this->ScaleOrientation = scaleOrientation;
}

vtkVector4f vtkTransform::GetScaleOrientation()
{
  return this->ScaleOrientation;
}

void vtkTransform::SetTranslation(vtkVector3f translation)
{
  this->Translation = translation;
}

vtkVector3f vtkTransform::GetTranslation()
{
  return this->Translation;
}
