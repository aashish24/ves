//
//  vtkActor.h
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkGMTL.h"
#include "vtkShaderProgram.h"
#include "vtkFileReader.h"
#include "vtkMapper.h"
class vtkActor
{
public:
  vtkActor(vtkMapper* mapper);
  ~vtkActor();
  void Read();
  void Print(vtkShaderProgram *program);
  
  vtkVector3f center;
  float radius;
  
 vtkMatrix4x4f mMatrix;
protected:
  void ComputeCenterAndRadius(vtkVector3f min, vtkVector3f max);
  vtkMapper* mMapper;
 };

