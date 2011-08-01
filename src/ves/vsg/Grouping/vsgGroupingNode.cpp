// ============================================================================
/**
 * @file   vsgGroupingNode.cpp
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
# include "vsgGroupingNode.h"
// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------cnstr
vsgGroupingNode::vsgGroupingNode()
{
}

// -----------------------------------------------------------------------destr
vsgGroupingNode::~vsgGroupingNode()
{
}

// ----------------------------------------------------------------------public
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

// ----------------------------------------------------------------------public
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
