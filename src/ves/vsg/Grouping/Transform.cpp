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

#include "Transform.h"

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
  vesMatrix4x4f Eval()
  {
    return T * C * R * SR * S * SRinv * Cinv;
  }

  void SetTranslation(SFVec3f trans)
  {
    T = makeTranslationMatrix4x4(trans);
  }

  void SetCenter(SFVec3f center)
  {
    C = makeTranslationMatrix4x4(center);
    Cinv = makeInverseMatrix4x4(C);
  }

  void SetRotation(SFRotation rot)
  {
    R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
  }

  void SetScaleOrientation(SFVec4f sr)
  {
    SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
    SRinv = makeInverseMatrix4x4(SR);
  }

  void SetScale(SFVec3f scale)
  {
    S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
  }
};

Transform::Transform()
{
  _internal = new TransformInternal();
  _center = SFVec3f (0,0,0);
  _rotation = SFRotation(0,0,1,0);
  _scale = SFVec3f(1,1,1);
  _scaleOrientation =  SFVec4f(0,0,1,0);
  _translation = SFVec3f(0,0,0);
}

Transform::~Transform()
{
  delete _internal;
}

void Transform::setInternals()
{
  _internal->SetTranslation(_translation);
  _internal->SetCenter(_center);
  _internal->SetRotation(_rotation);
  _internal->SetScaleOrientation(_scaleOrientation);
  _internal->SetScale(_scale);
}

SFMatrix4f Transform::computeTransform()
{
  setInternals();
  return _internal->Eval();
}

SFMatrix4f Transform::eval()
{
  setInternals();
  return _internal->Eval();
}

bool Transform::accept(vsgVisitor* vsgVisitor)
{
  return vsgVisitor->visitTransform(this);
}

}
