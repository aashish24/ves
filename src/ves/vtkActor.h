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
#include "vtkTransform.h"
#include <list>

class vtkShader;
class vtkMapper;
class vtkShader;
// -----------------------------------------------------------------------class
class vtkActor : public vtkTransform
{
public:
  vtkActor(); // needs to go after scene graph rendering
  vtkActor(vtkShader* shader,vtkMapper* mapper);
  ~vtkActor();
  vtkMatrix4x4f Eval();
  void Read();
  void Render(vtkShaderProgram *program); // needs to go after SG rendering
  void AddChild(vtkActor* actor);
private:
  
protected:
  vtkMapper* Mapper;
  vtkShader* Shader;
 };

#endif
