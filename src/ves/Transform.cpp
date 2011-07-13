
// --------------------------------------------------------------------includes
#include "Transform.h"
#include "vesGMTL.h"
#include "Painter.h"

// --------------------------------------------------------------------internal
class TransformInternals
{
  vesMatrix4x4f T,C,R,SR,S,_SR,_C;
public:
  vesMatrix4x4f Eval()
  {
    return T * C * R * SR * S * _SR * _C;
  }

  void SetTranslation(vesVector3f trans)
  {
    T = makeTranslationMatrix4x4(trans);
  }
  void SetCenter(vesVector3f center)
  {
    C = makeTranslationMatrix4x4(center);
    _C = makeInverseMatrix4x4(C);
  }
  void SetRotation(vesVector4f rot)
  {
    R = makeRotationMatrix4x4(rot[3], rot[0], rot[1], rot[2]);
  }

  void SetScaleOrientation(vesVector4f sr)
  {
    SR = makeRotationMatrix4x4(sr[3], sr[0], sr[1], sr[2]);
    _SR = makeInverseMatrix4x4(SR);
  }

  void SetScale(vesVector3f scale)
  {
    S = makeScaleMatrix4x4(scale[0],scale[1],scale[2]);
  }
};

// -----------------------------------------------------------------------cnstr
Transform::Transform()
{
  this->Internals = new TransformInternals;
  vesVector3f center(0,0,0);
  vesVector4f rotation(0,0,1,0);
  vesVector3f scale(1,1,1);
  vesVector4f scaleOrientation(0,0,1,0);
  vesVector3f translation(0,0,0);
  this->Center = center;
  this->Rotation = rotation;
  this->Scale = scale;
  this->ScaleOrientation = scaleOrientation;
  this->Translation = translation;
}

// -----------------------------------------------------------------------destr
Transform::~Transform()
{
  delete this->Internals;
}

// ----------------------------------------------------------------------public
vesMatrix4x4f Transform::Eval()
{
  this->SetInternals();
  return this->Internals->Eval();
}

// ----------------------------------------------------------------------public
// void Transform::Render(Painter *render)
// {
//   render->Transform(this);
// }

// ---------------------------------------------------------------------private
vesMatrix4x4f Transform::ComputeTransform()
{
  this->SetInternals();
  return this->Internals->Eval();
}

// ---------------------------------------------------------------------private
void Transform::SetInternals()
{
  this->Internals->SetTranslation(this->Translation);
  this->Internals->SetCenter(this->Center);
  this->Internals->SetRotation(this->Rotation);
  this->Internals->SetScaleOrientation(this->ScaleOrientation);
  this->Internals->SetScale(this->Scale);
}

// void Transform::Handle(vesController *handle)
// {
//   handle->Transform(this);
// }
