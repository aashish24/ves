#ifndef __vesCamera_h
#define __vesCamera_h

#include "vesGMTL.h"
#include "Transform.h"
#include "vesSetGet.h"
#include "vesActorCollection.h"

#define VTK 0

class vesCamera: public Transform
{
public:
  vesCamera();
  ~vesCamera();
  bool Read();
  void Render(Painter *render);
  void Normalize();
  void ComputeBounds();
  void AddActorCollection(vesActorCollection* actor);
  vesMatrix4x4f Eval();

  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)

#if VTK
  vesSetGetMacro(UseHorizontalViewAngle,bool)
  vesSetGetMacro(ViewPlaneNormal,vesVector3f)
  vesSetGetMacro(ViewAngle, float)
  vesSetGetMacro(Position, vesVector3f)
  vesSetGetMacro(FocalPoint, vesVector3f)
  vesSetGetMacro(ViewUp, vesVector3f)
  vesSetGetMacro(ParallelScale, float)

  vesMatrix4x4f ComputeViewTransform();
  vesMatrix4x4f ComputeProjectionTransform(float aspect, float near, float far);
  void SetWindowCenter(double x, double y);
  void SetClippingRange(float near, float far);
#endif

private:
  float ViewAngle;
  bool UseHorizontalViewAngle;
  vesVector3f ViewPlaneNormal;
  vesVector3f Position, FocalPoint,ViewUp;
  float ParallelScale;
  float ClippingRange[2];
  void Reset();
  void ComputeDistance();
  void ComputeViewPlaneNormal();
  float Distance;
  vesVector3f DirectionOfProjection;
  double WindowCenter[2];
  bool ParallelProjection;

protected:
  // ...........................................................protected-ivars

  vesMatrix4x4f NormalizedMatrix;

};
#endif //__vesCamera_h
