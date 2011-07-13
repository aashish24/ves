#ifndef __Transform_h
#define __Transform_h

#include "vsgGroupingNode.h"

class TransformInternals;

class Transform: public vsgGroupingNode
{
public:
  Transform();
  virtual ~Transform();
  //void Handle(vesController *handle);
  //void Render(Painter *render);
  vesMatrix4x4f Eval();
  vesSetGetMacro(Center,vesVector3f)
  vesSetGetMacro(Rotation, vesVector4f)
  vesSetGetMacro(Scale, vesVector3f)
  vesSetGetMacro(ScaleOrientation, vesVector4f)
  vesSetGetMacro(Translation,vesVector3f)

private:
  void SetInternals();
  vesMatrix4x4f ComputeTransform();

protected:
  vesVector3f Center;
  vesVector4f Rotation;
  vesVector3f Scale;
  vesVector4f ScaleOrientation;
  vesVector3f Translation;
  TransformInternals *Internals;
};

#endif
