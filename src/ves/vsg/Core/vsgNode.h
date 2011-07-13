//
//  vsgNode.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
#ifndef __vsgNode_h
#define __vsgNode_h

//#include "Painter.h"
class Painter;
class vsgNode
{
public:
  vsgNode();
  virtual ~vsgNode();
  virtual bool Read()=0;
  //virtual void Handle(vesController *handle) = 0;
  virtual void Render(Painter *render)=0;
};
#endif
