// ============================================================================
/**
 * @file   vsgBoundedObject.cpp
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
# include "vsgBoundedObject.h"

// -----------------------------------------------------------------------cnstr
vsgBoundedObject::vsgBoundedObject()
{
  this->_BBoxCenter[0] = 0;
  this->_BBoxCenter[1] = 0;
  this->_BBoxCenter[2] = 0;
  this->_BBoxSize[0] = -1;
  this->_BBoxSize[1] = -1;
  this->_BBoxSize[2] = -1;
  this->_min[0] = 0;
  this->_min[1] = 0;
  this->_min[2] = 0;
  this->_max[0] = 0;
  this->_max[1] = 0;
  this->_max[2] = 0;
}

// -----------------------------------------------------------------------destr
vsgBoundedObject::~vsgBoundedObject()
{
}

void vsgBoundedObject::set_BBoxCenter(vesVector3f min, vesVector3f max)
{
  this->_min = (min);
  this->_max = (max);
  this->_BBoxCenter = max + min;
  this->_BBoxCenter /= 2.0f;
}

void vsgBoundedObject::set_BBoxSize(vesVector3f min, vesVector3f max)
{
  this->_min = (min);
  this->_max = (max);
  this->_BBoxSize = max-min;
  for (int i=0; i<3; ++i) {
    if(this->_BBoxSize[i]<0)
    {
      this->_BBoxSize *= -1;
    }
  }
}

float vsgBoundedObject::GetBBoxRadius()
{
  vesVector3f temp = this->_BBoxSize;
  return sqrt(temp[0]*temp[0]+
              temp[1]*temp[1]+
              temp[2]*temp[2]);
}
