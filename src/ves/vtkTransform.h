#ifndef __vtkTransform_h
#define __vtkTransform_h

#include "vtkGroupingNode.h"

class vtkTransformInternals;

class vtkTransform: public vtkGroupingNode
{
public:
  vtkTransform();
  virtual ~vtkTransform();
  //void Handle(vtkController *handle);
  //void Render(vtkPainter *render);
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
  vtkTransformInternals *Internals;
};

#endif
