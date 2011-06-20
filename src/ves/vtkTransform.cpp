
// --------------------------------------------------------------------includes
#include "vtkTransform.h"
#include "vtkGMTL.h"
#include "vtkPainter.h"

// --------------------------------------------------------------------internal
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

// -----------------------------------------------------------------------cnstr
vtkTransform::vtkTransform()
{
  this->Internals = new vtkTransformInternals;
  vtkVector3f center(0,0,0);
  vtkVector4f rotation(0,0,1,0);
  vtkVector3f scale(1,1,1);
  vtkVector4f scaleOrientation(0,0,1,0);
  vtkVector3f translation(0,0,0);
  this->Center = center;
  this->Rotation = rotation;
  this->Scale = scale;
  this->ScaleOrientation = scaleOrientation;
  this->Translation = translation;
}

// -----------------------------------------------------------------------destr
vtkTransform::~vtkTransform()
{
  delete this->Internals;
}

// ----------------------------------------------------------------------public
vtkMatrix4x4f vtkTransform::Eval()
{
  this->SetInternals();
  return this->Internals->Eval();
}

// ----------------------------------------------------------------------public
// void vtkTransform::Render(vtkPainter *render)
// {
//   render->Transform(this);
// }

// ---------------------------------------------------------------------private
vtkMatrix4x4f vtkTransform::ComputeTransform()
{
  this->SetInternals();
  return this->Internals->Eval();
}

// ---------------------------------------------------------------------private
void vtkTransform::SetInternals()
{
  this->Internals->SetTranslation(this->Translation);
  this->Internals->SetCenter(this->Center);
  this->Internals->SetRotation(this->Rotation);
  this->Internals->SetScaleOrientation(this->ScaleOrientation);
  this->Internals->SetScale(this->Scale);
}

// void vtkTransform::Handle(vtkController *handle)
// {
//   handle->Transform(this);
// }
