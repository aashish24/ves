//
//  vtkActor.h
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef __vtkActor_h
#define __vtkActor_h

#include "vtkGMTL.h"
#include "vtkShaderProgram.h"
#include "vtkFileReader.h"
#include "vtkMapper.h"
#include "vtkBoundedObject.h"
#include <list>

class vtkActor : public vtkBoundedObject
{
public:
  vtkActor(vtkMapper* mapper);
  ~vtkActor();
  vtkMatrix4x4f operator ()();
  void Read();
  vtkMatrix4x4f Eval();
  void Print(vtkShaderProgram *program);
private:
  vtkPoint3f GetMin();
  vtkPoint3f GetMax();
protected:
  void ComputeCenterAndRadius(vtkPoint3f min, vtkPoint3f max);
  vtkVector3f center;
  float radius;
  vtkMatrix4x4f mMatrix;
  vtkMapper* mMapper;
 };

#endif
