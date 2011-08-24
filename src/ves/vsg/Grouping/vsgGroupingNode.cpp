/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/

# include "vsgGroupingNode.h"

vsgGroupingNode::vsgGroupingNode()
{
}

vsgGroupingNode::~vsgGroupingNode()
{
}

void vsgGroupingNode::addChildren(MFNode &children)
{
  for (int i=0; i<children.size(); i++)
  {
    vsgChildNode *current = (vsgChildNode*) children[i];
    bool present = false;
    for (int j =0; j< _children.size(); j++)
    {
      if(current == _children[j])
      {
        present = true;
        break;
      }
    }
    if(!present)
    {
      _children.push_back(current);
    }
  }
}

void vsgGroupingNode::removeChildren(MFNode &children)
{
  for (int i=0; i<children.size(); i++)
  {
    vsgChildNode *current = (vsgChildNode*) children[i];
    bool present = false;
    int j;
    for (j =0; j< _children.size(); j++) {
      if(current == _children[j])
      {
        present = true;
        break;
      }
    }
    if(present)
    {
      _children.erase(_children.begin()+j);
    }
  }
}
