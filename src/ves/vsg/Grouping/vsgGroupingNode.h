//
//  vsgGroupingNode.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vsgChildNode.h"
#include "vsgBoundedObject.h"
#include <vector>
#include "vesSetGet.h"


class vsgGroupingNode: public vsgChildNode , public vsgBoundedObject
{
public:
  vsgGroupingNode();
  virtual ~vsgGroupingNode();
  void AddChildren(const std::vector<vsgChildNode*> &children);
  void RemoveChildren(const std::vector<vsgChildNode*> &children);
  vesSetGetVectorMacro(Children, vsgChildNode*)

  protected:
  std::vector<vsgChildNode*> Children;
};

