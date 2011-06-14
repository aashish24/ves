//
//  vtkGroupingNode.cpp
//  kiwi
//
//  Created by kitware on 6/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "vtkGroupingNode.h"


vtkGroupingNode::vtkGroupingNode()
{
  
}

vtkGroupingNode::~vtkGroupingNode()
{
  
}

void vtkGroupingNode::AddChildren(const std::vector<vtkChildNode*> &children)
{
  for (int i=0; i<children.size(); i++) 
  {
    vtkChildNode *current = children[i];
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

void vtkGroupingNode::RemoveChildren(const std::vector<vtkChildNode*> &children)
{
  for (int i=0; i<children.size(); i++) 
  {
    vtkChildNode *current = children[i];
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

void vtkGroupingNode::SetChildren(const std::vector<vtkChildNode*> &children)
{
  // Copy all the elements into the object
  for(int i =0; i< children.size(); i++)
  {
    this->Children[i] = children[i];
  }
}

bool vtkGroupingNode::GetChildren (std::vector<vtkChildNode*> &children)
{
  // if there are no children the return false
  if(children.size() ==0)
    return false;
  
  // Copy all the children nodes
  for(int ii=0; ii < this->Children.size(); ii++)
  {
    children[ii] = this->Children[ii];
  }
  
  // Copy is done
  return true;
}