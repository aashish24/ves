#include "vesCamera.h"
#include "vesGMTL.h"
#include <iostream>
#include "Painter.h"

// -----------------------------------------------------------------------cnstr
vesCamera::vesCamera()
{
}

// -----------------------------------------------------------------------destr
vesCamera::~vesCamera()
{

}

#if VTK
// ----------------------------------------------------------------------public
vesMatrix4x4f vesCamera::ComputeViewTransform()
{
  return vesLookAt (this->Position,
		    this->FocalPoint,
		    this->ViewUp);
}

// ----------------------------------------------------------------------public
vesMatrix4x4f vesCamera::ComputeProjectionTransform(float aspect,
                                                    float nearz,
                                                    float farz)
{
  vesMatrix4x4f matrix;

  // adjust Z-buffer range
  matrix[2][2] = (farz - nearz)/(1 - (-1));
  matrix[2][3] = (nearz*1 - farz*(-1))/(1 - (-1));


  if ( this->ParallelProjection)
    {
    // set up a rectangular parallelipiped

    double width = this->ParallelScale * aspect;
    double height = this->ParallelScale;

    double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
    double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
    double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
    double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

    return matrix * vesOrtho( xmin, xmax, ymin, ymax,
                              this->ClippingRange[0],
                              this->ClippingRange[1] );
    }
  else
    {
      // set up a perspective frustum
      double tmp = tan( deg2Rad( this->ViewAngle ) / 2. );
      double width;
      double height;
      if ( this->UseHorizontalViewAngle )
        {
        width = this->ClippingRange[0] * tmp;
        height = this->ClippingRange[0] * tmp / aspect;
        }
      else
        {
        width = this->ClippingRange[0] * tmp * aspect;
        height = this->ClippingRange[0] * tmp;
        }

      double xmin = ( this->WindowCenter[0] - 1.0 ) * width;
      double xmax = ( this->WindowCenter[0] + 1.0 ) * width;
      double ymin = ( this->WindowCenter[1] - 1.0 ) * height;
      double ymax = ( this->WindowCenter[1] + 1.0 ) * height;

      return matrix*vesFrustum( xmin, xmax, ymin, ymax,
      this->ClippingRange[0],
      this->ClippingRange[1] );
    }
}
//----------------------------------------------------------------------------
void vesCamera::SetWindowCenter(double x, double y)
{
  this->WindowCenter[0] = x;
  this->WindowCenter[1] = y;
}

// ----------------------------------------------------------------------public
void vesCamera::SetClippingRange(float near, float far)
{
  this->ClippingRange[0] = near;
  this->ClippingRange[1] = far;
}
#endif
//----------------------------------------------------------------------------
// This method must be called when the focal point or camera position changes
void vesCamera::ComputeDistance()
{
  double dx = this->FocalPoint[0] - this->Position[0];
  double dy = this->FocalPoint[1] - this->Position[1];
  double dz = this->FocalPoint[2] - this->Position[2];

  this->Distance = sqrt(dx*dx + dy*dy + dz*dz);

  if (this->Distance < 1e-20)
  {
    this->Distance = 1e-20;
    vesVector3f vec = this->DirectionOfProjection;

    // recalculate FocalPoint
    this->FocalPoint[0] = this->Position[0] + vec[0]*this->Distance;
    this->FocalPoint[1] = this->Position[1] + vec[1]*this->Distance;
    this->FocalPoint[2] = this->Position[2] + vec[2]*this->Distance;
  }

  this->DirectionOfProjection[0] = dx/this->Distance;
  this->DirectionOfProjection[1] = dy/this->Distance;
  this->DirectionOfProjection[2] = dz/this->Distance;

  this->ComputeViewPlaneNormal();
}

// ----------------------------------------------------------------------public
void vesCamera::ComputeViewPlaneNormal()
{
    this->ViewPlaneNormal[0] = -this->DirectionOfProjection[0];
    this->ViewPlaneNormal[1] = -this->DirectionOfProjection[1];
    this->ViewPlaneNormal[2] = -this->DirectionOfProjection[2];
}

// ----------------------------------------------------------------------public
void vesCamera::Render(Painter *render)
{
  render->Camera(this);
}

// ----------------------------------------------------------------------public
void vesCamera::ComputeBounds()
{
  vesVector3f allMin(0,0,0);
  vesVector3f allMax(0,0,0);

  for (int i =0; i<this->Children.size(); ++i)
    {
    vesActorCollection* child = (vesActorCollection*) this->Children[i];
    child->ComputeBounds();
    vesVector3f min = child->GetMin();
    vesVector3f max = child->GetMax();

    if (i == 0)
      {
      allMin = min;
      allMax = max;
      }

    for (int i = 0; i < 3; ++i)
      {
      if (max[i] > allMax[i])
        {
        allMax[i] = max[i];
        }
      if (min[i] < allMin[i])
        {
        allMin[i] = min[i];
        }
      }
    }

  SetBBoxCenter(allMin, allMax);
  SetBBoxSize(allMin, allMax);
}

// ----------------------------------------------------------------------public
void vesCamera::Normalize()
{
  float r = GetBBoxRadius();
  this->NormalizedMatrix =
  makeScaleMatrix4x4(1/r,1/r,1/r)*
  makeTranslationMatrix4x4(-GetBBoxCenter());
  SetBBoxCenter(transformPoint3f(this->NormalizedMatrix, GetBBoxCenter()));
  SetBBoxSize(transformPoint3f(this->NormalizedMatrix, GetBBoxSize()));
}

// ----------------------------------------------------------------------public
bool vesCamera::Read()
{
  for (int i =0; i<this->Children.size(); ++i)
  {
    vesActorCollection* child = (vesActorCollection*) this->Children[i];
    child->Read();
  }
  return true;
}

void vesCamera::AddActorCollection(vesActorCollection* actor)
{
  std::vector<vsgChildNode*> actorList;
  actorList.push_back(actor);
  AddChildren(actorList);
}

vesMatrix4x4f vesCamera::Eval()
{
  return NormalizedMatrix*Transform::Eval();
}
