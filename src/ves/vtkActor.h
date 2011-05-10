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

class vtkActor
{
public:
  vtkActor(vtkFileReader* reader);
  void Release();
  void Read();
  void Print(vtkShaderProgram *program);
  
  vtkVector3f center;
  float radius;
  
protected:
  void ComputeCenterAndRadius(vtkVector3f min, vtkVector3f max);
  vtkMatrix4x4f mMatrix;
  bool once;
  vtkFileReader *mFileReader;
  bool m_initialized;
  unsigned int mMapperVBO[2];
};

