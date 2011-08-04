#include "vesCamera.h"
#include "vesGMTL.h"
#include <iostream>
#include "Painter.h"
#include "gmtl/Generate.h"

namespace {
void PrintMatrix(std::string name, vesMatrix4x4f mv)
{
  std::cerr << name << ":" << std::endl;
  for (int i = 0; i < 4; ++i)
    {
    std::cerr << mv[i][0] << "," << mv[i][1] << "," << mv[i][2] << "," << mv[i][3] << std::endl;
    }
  std::cerr << std::endl;
}
}

// -----------------------------------------------------------------------cnstr
vesCamera::vesCamera()
{
  this->FocalPoint[0] = 0.0;
  this->FocalPoint[1] = 0.0;
  this->FocalPoint[2] = 0.0;

  this->Position[0] = 0.0;
  this->Position[1] = 0.0;
  this->Position[2] = 1.0;

  this->ViewUp[0] = 0.0;
  this->ViewUp[1] = 1.0;
  this->ViewUp[2] = 0.0;

  this->DirectionOfProjection[0] = 0.0;
  this->DirectionOfProjection[1] = 0.0;
  this->DirectionOfProjection[2] = 0.0;

  this->ViewAngle = 30.0;
  this->UseHorizontalViewAngle = 0;

  //this->ClippingRange[0] = 0.01;
  //this->ClippingRange[1] = 1000.01;
  this->ClippingRange[0] = 10.0;
  this->ClippingRange[1] = 1010.0;

  this->ParallelProjection = 0;
  this->ParallelScale = 1.0;

  this->WindowCenter[0] = 0.0;
  this->WindowCenter[1] = 0.0;

  this->ComputeDistance();
}

// -----------------------------------------------------------------------destr
vesCamera::~vesCamera()
{
}

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

    vesMatrix4x4f ortho = vesOrtho( xmin, xmax, ymin, ymax,
                                   this->ClippingRange[0],
                                   this->ClippingRange[1] );
    return matrix * ortho;
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

      vesMatrix4x4f frustum = vesFrustum( xmin, xmax, ymin, ymax,
                                         this->ClippingRange[0],
                                         this->ClippingRange[1] );

      return matrix*frustum;
    }
}

//----------------------------------------------------------------------------
// Rotate the camera about the view up vector centered at the focal point.
void vesCamera::Azimuth(double angle)
{
  vesVector3f fp = this->FocalPoint;
  vesVector3f vu = this->ViewUp;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(deg2Rad(angle), vu[0], vu[1], vu[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesMatrix4x4f t = t1 * t2 * t3;

  vesVector4f oldPosition(this->Position[0], this->Position[1], this->Position[2], 1);
  vesVector4f newPosition;
  gmtl::xform(newPosition, t, oldPosition);
  this->Position[0] = newPosition[0] / newPosition[3];
  this->Position[1] = newPosition[1] / newPosition[3];
  this->Position[2] = newPosition[2] / newPosition[3];
  this->ComputeDistance();
}

//----------------------------------------------------------------------------
// Rotate the camera about the cross product of the negative of the
// direction of projection and the view up vector centered on the focal point.
void vesCamera::Elevation(double angle)
{
  vesMatrix4x4f view = this->ComputeViewTransform();
  vesVector3f axis;
  axis[0] = -view[0][0];
  axis[1] = -view[0][1];
  axis[2] = -view[0][2];
  vesVector3f fp = this->FocalPoint;
  vesVector3f nfp(-fp[0], -fp[1], -fp[2]);
  vesMatrix4x4f t1 = makeTranslationMatrix4x4(fp);
  vesMatrix4x4f t2 = makeRotationMatrix4x4(-deg2Rad(angle), axis[0], axis[1], axis[2]);
  vesMatrix4x4f t3 = makeTranslationMatrix4x4(nfp);
  vesMatrix4x4f t = t1 * t2 * t3;

  vesVector4f oldPosition(this->Position[0], this->Position[1], this->Position[2], 1);
  vesVector4f newPosition;
  gmtl::xform(newPosition, t, oldPosition);
  this->Position[0] = newPosition[0] / newPosition[3];
  this->Position[1] = newPosition[1] / newPosition[3];
  this->Position[2] = newPosition[2] / newPosition[3];
  this->ComputeDistance();
}

// ----------------------------------------------------------------------public
void vesCamera::Dolly(double factor)
{
  if (factor <= 0.0)
    {
    return;
    }

  // dolly moves the camera towards the focus
  double d = this->Distance/factor;

  this->Position[0] = this->FocalPoint[0] - d*this->DirectionOfProjection[0];
  this->Position[1] = this->FocalPoint[1] - d*this->DirectionOfProjection[1];
  this->Position[2] = this->FocalPoint[2] - d*this->DirectionOfProjection[2];
  this->ComputeDistance();
}

//-----------------------------------------------------------------------public
void vesCamera::Roll(double angle)
{
  // rotate ViewUp about the Direction of Projection
  vesMatrix4x4f t = makeRotationMatrix4x4(-deg2Rad(angle),
                                          this->DirectionOfProjection[0],
                                          this->DirectionOfProjection[1],
                                          this->DirectionOfProjection[2]);
  vesVector3f newViewUp;
  gmtl::xform(newViewUp, t, this->ViewUp);
  this->SetViewUp(newViewUp);
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

//----------------------------------------------------------------------------
void vesCamera::OrthogonalizeViewUp()
{
// the orthogonalized ViewUp is just the second row of the view matrix
  vesMatrix4x4f view = this->ComputeViewTransform();
  this->ViewUp[0] = view[1][0];
  this->ViewUp[1] = view[1][1];
  this->ViewUp[2] = view[1][2];
}

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

  for (int i =0; i<this->get_children().size(); ++i)
    {
      vesActorCollection* child = (vesActorCollection*) this->get_children()[i];
    child->ComputeBounds();
    vesVector3f min = child->get_min();
    vesVector3f max = child->get_max();

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

  set_BBoxCenter(allMin, allMax);
  set_BBoxSize(allMin, allMax);
}

// ----------------------------------------------------------------------public
bool vesCamera::Read()
{
  for (int i =0; i<this->get_children().size(); ++i)
  {
    vesActorCollection* child = (vesActorCollection*) this->get_children()[i];
    child->Read();
  }
  return true;
}

void vesCamera::AddActorCollection(vesActorCollection* actor)
{
  MFNode actorList;
  actorList.push_back(actor);
  addChildren(actorList);
}
