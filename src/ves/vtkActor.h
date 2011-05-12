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
  float radius;
  vtkMatrix4x4f mMatrix;
  vtkPoint3f GetMin()
  {
   return transformPoint3f(mMatrix ,mMapper->GetMin()); 
  }
  
  vtkPoint3f GetMax()
  {
    return transformPoint3f(mMatrix ,mMapper->GetMax());  
  }
protected:
  void ComputeCenterAndRadius(vtkPoint3f min, vtkPoint3f max);
  vtkMapper* mMapper;
  vtkVector3f center;
  
 };

