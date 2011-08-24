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

# include "vsgBoundedObject.h"

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
