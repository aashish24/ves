//
//  vtkGroupingNode.h
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkChildNode.h"
#include "vtkBoundedObject.h"
#include <vector>
#include "vtkSetGet.h"


class vtkGroupingNode: public vtkChildNode , public vtkBoundedObject
{
public:
  vtkGroupingNode();
  virtual ~vtkGroupingNode();
  void AddChildren(const std::vector<vtkChildNode*> &children);
  void RemoveChildren(const std::vector<vtkChildNode*> &children);
  vtkSetGetVectorMacro(Children, vtkChildNode*)

  protected:
  std::vector<vtkChildNode*> Children;
};

