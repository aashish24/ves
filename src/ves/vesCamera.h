#ifndef __vesCamera_h
#define __vesCamera_h

#include "vesGMTL.h"
#include "Transform.h"
#include "vesSetGet.h"
#include "vesActorCollection.h"

class vesCamera: public Transform
{
public:
  vesCamera();
  ~vesCamera();
  bool Read();
  void Render(Painter *render);
  void ComputeBounds();
  void AddActorCollection(vesActorCollection* actor);

  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)

  vesSetGetMacro(UseHorizontalViewAngle,bool)
  vesSetGetMacro(ViewPlaneNormal,vesVector3f)
  vesSetGetMacro(ViewAngle, float)
  void SetPosition(vesVector3f pos) { Position = pos; ComputeDistance(); }
  vesVector3f GetPosition() { return Position; }
  vesSetGetMacro(FocalPoint, vesVector3f)
  vesSetGetMacro(ViewUp, vesVector3f)
  vesSetGetMacro(ParallelScale, float)

  void Azimuth(double angle);
  void Elevation(double angle);
  void Dolly(double factor);
  void OrthogonalizeViewUp();

  vesMatrix4x4f ComputeViewTransform();
  vesMatrix4x4f ComputeProjectionTransform(float aspect, float near, float far);
  void SetWindowCenter(double x, double y);
  void SetClippingRange(float near, float far);
  void Reset();

private:
  float ViewAngle;
  bool UseHorizontalViewAngle;
  vesVector3f ViewPlaneNormal;
  vesVector3f Position, FocalPoint,ViewUp;
  float ParallelScale;
  float ClippingRange[2];
  void ComputeDistance();
  void ComputeViewPlaneNormal();
  float Distance;
  vesVector3f DirectionOfProjection;
  double WindowCenter[2];
  bool ParallelProjection;
};
#endif //__vesCamera_h
