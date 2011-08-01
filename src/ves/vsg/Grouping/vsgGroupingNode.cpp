//
//  vsgGroupingNode.cpp
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vsgGroupingNode.h"


vsgGroupingNode::vsgGroupingNode()
{

}

vsgGroupingNode::~vsgGroupingNode()
{

}

void vsgGroupingNode::AddChildren(const std::vector<vsgChildNode*> &children)
{
  for (int i=0; i<children.size(); i++)
  {
    vsgChildNode *current = children[i];
    bool present = false;
    for (int j =0; j< this->Children.size(); j++)
    {
      if(current == this->Children[j])
      {
        present = true;
        break;
      }
    }
    if(!present)
    {
      this->Children.push_back(current);
    }
  }
}

void vsgGroupingNode::RemoveChildren(const std::vector<vsgChildNode*> &children)
{
  for (int i=0; i<children.size(); i++)
  {
    vsgChildNode *current = children[i];
    bool present = false;
    int j;
    for (j =0; j< this->Children.size(); j++) {
      if(current == this->Children[j])
      {
        present = true;
        break;
      }
    }
    if(present)
    {
      this->Children.erase(this->Children.begin()+j);
    }
  }
}

