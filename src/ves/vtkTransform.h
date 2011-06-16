//
//  vtkTransform.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkTransform_h
#define __vtkTransform_h

#include "vtkGroupingNode.h"

class vtkTransformInternals;

class vtkTransform: public vtkGroupingNode
{
public:
  vtkTransform();
  virtual ~vtkTransform();
  
  vtkMatrix4x4f Eval();
  
  void SetCenter(vtkVector3f center);
  vtkVector3f GetCenter();
  void SetRotation(vtkVector4f rotation);
  vtkVector4f GetRotation();
  void SetScale(vtkVector3f scale);
  vtkVector3f GetScale();
  void SetScaleOrientation(vtkVector4f scaleOrientation);
  vtkVector4f GetScaleOrientation();
  void SetTranslation(vtkVector3f translation);
  vtkVector3f GetTranslation();
  
private:
  void SetInternals();
  
protected:
  vtkVector3f Center;
  vtkVector4f Rotation;
  vtkVector3f Scale;
  vtkVector4f ScaleOrientation;
  vtkVector3f Translation;
  vtkTransformInternals *Internals;
};

#endif