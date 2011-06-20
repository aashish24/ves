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
  vtkMatrix4x4f Eval();
  vtkSetGetMacro(Center,vtkVector3f)
  vtkSetGetMacro(Rotation, vtkVector4f)
  vtkSetGetMacro(Scale, vtkVector3f)
  vtkSetGetMacro(ScaleOrientation, vtkVector4f)
  vtkSetGetMacro(Translation,vtkVector3f)

private:
  void SetInternals();
  vtkMatrix4x4f ComputeTransform();

protected:
  vtkVector3f Center;
  vtkVector4f Rotation;
  vtkVector3f Scale;
  vtkVector4f ScaleOrientation;
  vtkVector3f Translation;
  vtkTransformInternals *Internals;
};

#endif
