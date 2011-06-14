//
//  vtkNode.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

class vtkNode
{
public:
  vtkNode();
  virtual void Eval()=0;
  virtual ~vtkNode();
}; 
