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

#include "vesBoundingObject.h"

// C/C++ includes
#include <limits>

vesBoundingObject::vesBoundingObject() : vesObject(),
  m_boundsDirty(true)
{
  this->resetBounds();
}


vesBoundingObject::~vesBoundingObject()
{
}


void vesBoundingObject::setBoundsCenter(const vesVector3f &center)
{
    this->m_boundsCenter = center;
}


void vesBoundingObject::setBoundsSize(const vesVector3f &size)
{
    this->m_boundsSize = size;
}


void vesBoundingObject::setBounds(vesVector3f min, vesVector3f max)
{
  this->m_boundsMinimum = min;
  this->m_boundsMaximum = max;

  this->m_boundsCenter = 0.5f * (max + min);

  this->m_boundsSize = max - min;
}


float vesBoundingObject::boundsRadius()
{
  vesVector3f temp = this->m_boundsSize;

  return sqrt(temp[0]*temp[0]+
              temp[1]*temp[1]+
              temp[2]*temp[2]);
}


void vesBoundingObject::setBoundsDirty(bool value)
{
  this->m_boundsDirty = value;
}


void vesBoundingObject::resetBounds()
{
  this->m_boundsCenter[0] = 0;
  this->m_boundsCenter[1] = 0;
  this->m_boundsCenter[2] = 0;

  this->m_boundsSize[0] = 0;
  this->m_boundsSize[1] = 0;
  this->m_boundsSize[2] = 0;

  this->m_boundsMinimum[0] = std::numeric_limits<float>::max();
  this->m_boundsMinimum[1] = std::numeric_limits<float>::max();
  this->m_boundsMinimum[2] = std::numeric_limits<float>::max();

  // std::numeric_limits<float>::min() returns lowest possible absolute value.
  // As far as the compiler / platform support IEEE floating point (754) (most of them)
  // the code below should work. in C++11 we have ::lowest().
  this->m_boundsMaximum[0] = -std::numeric_limits<float>::max();
  this->m_boundsMaximum[1] = -std::numeric_limits<float>::max();
  this->m_boundsMaximum[2] = -std::numeric_limits<float>::max();
}
