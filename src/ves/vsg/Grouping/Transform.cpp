// ============================================================================
/**
 * @file   Transform.cpp
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
# include "Transform.h"
// --------------------------------------------------------------------includes
#include "vsg/vsgVisitor.h"

namespace vsg{
// ....................................................................internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
class TransformInternal
{
private:
  vesMatrix4x4f T,C,R,SR,S,SRinv,Cinv;
public:
  // ..................................................................internal
  vesMatrix4x4f Eval()
    {
      return T * C * R * SR * S * SRinv * Cinv;
    }
  // ..................................................................internal
  void SetTranslation(SFVec3f trans)
    {
      T = makeTranslationMatrix4x4(trans);
    }
  // .................................................................internal
  void SetCenter(SFVec3f center)
    {
      C = makeTranslationMatrix4x4(center);
      Cinv = makeInverseMatrix4x4(C);
    }
  // ..................................................................internal
  void SetRotation(SFRotation rot)
    {
      R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
    }
  // ..................................................................internal
  void SetScaleOrientation(SFVec4f sr)
    {
      SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
      SRinv = makeInverseMatrix4x4(SR);
    }
  // ..................................................................internal
  void SetScale(SFVec3f scale)
    {
      S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
    }
};


// -----------------------------------------------------------------------cnstr
Transform::Transform()
{
  _internal = new TransformInternal();
  _center = SFVec3f (0,0,0);
  _rotation = SFRotation(0,0,1,0);
  _scale = SFVec3f(1,1,1);
  _scaleOrientation =  SFVec4f(0,0,1,0);
  _translation = SFVec3f(0,0,0);
}

// -----------------------------------------------------------------------destr
Transform::~Transform()
{
  delete _internal;
}

// ---------------------------------------------------------------------private
void Transform::setInternals()
{
  _internal->SetTranslation(_translation);
  _internal->SetCenter(_center);
  _internal->SetRotation(_rotation);
  _internal->SetScaleOrientation(_scaleOrientation);
  _internal->SetScale(_scale);
}

// ---------------------------------------------------------------------private
SFMatrix4f Transform::computeTransform()
{
  setInternals();
  return _internal->Eval();
}

// ----------------------------------------------------------------------public
SFMatrix4f Transform::eval()
{
  setInternals();
  return _internal->Eval();
}

// ----------------------------------------------------------------------public
bool Transform::accept(vsgVisitor* vsgVisitor)
  {
    return vsgVisitor->visitTransform(this);
  }
}
