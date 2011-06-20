//
//  vtkNode.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vtkNode_h
#define __vtkNode_h

//#include "vtkPainter.h"
class vtkPainter;
class vtkNode
{
public:
  vtkNode();
  virtual ~vtkNode();
  virtual bool Read()=0;
  //virtual void Handle(vtkController *handle) = 0;
  virtual void Render(vtkPainter *render)=0;
};
#endif
