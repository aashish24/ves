/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vsgSensorNode.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vsgSensorNode.h"

// --------------------------------------------------------------------includes

// -----------------------------------------------------------------------macro

// -----------------------------------------------------------------------cnstr
vsgSensorNode::vsgSensorNode()
{
  this->Enabled = false;
}

// -----------------------------------------------------------------------destr
vsgSensorNode::~vsgSensorNode()
{
}

bool vsgSensorNode::IsActive()
{
  return this->Enabled;
}



