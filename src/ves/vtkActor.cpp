//
//  vtkActor.cpp
//  kiwi
//
//  Created by kitware on 5/9/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkActor.h"

vtkActor::vtkActor(vtkMapper* mapper): mMapper(mapper)
{
}

vtkActor::~vtkActor()
{
  if(mMapper)
  {
  delete mMapper;
  }
}

void vtkActor::Read()
{
  mMapper->Read();
  ComputeCenterAndRadius(mMapper->GetMin(),
                         mMapper->GetMax());
  mMatrix = makeTranslationMatrix4x4(center)*
                makeScaleMatrix4x4(1/radius)*
            makeTranslationMatrix4x4(-center);
  mMatrix = mMatrix * makeTranslationMatrix4x4(-center);
 }

void vtkActor::Print(vtkShaderProgram *program)
{
  mMapper->Print(program);
  glVertexAttrib4f(program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
}

void vtkActor::ComputeCenterAndRadius(vtkVector3f min, vtkVector3f max)
{
  center[0] = (min[0]+max[0])/2;
  center[1] = (min[1]+max[1])/2;
  center[2] = (min[2]+max[2])/2;
  float x = max[0]-min[0];
  float y = max[1]-min[1];
  float z = max[2]-min[2];
  radius = sqrt(x*x+y*y+z*z);
  
  std::cout << "min    = ( "<< min[0] << " " << min[1] << " " << min[2] << ")" << std::endl;
  std::cout << "max    = ( "<< max[0] << " " << max[1] << " " << max[2] << ")" << std::endl;
  std::cout << "Center = ( "<< center[0] << " " << center[1] << " " << center[2] << ")" << std::endl;
  std::cout << "Radius = " << radius <<std::endl;
}

