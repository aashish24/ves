/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkFileReader.cpp
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkFileReader.h"

vtkFileReader::vtkFileReader(std::string fileName)
{
  this->mFileName = fileName;
}

vtkFileReader::~vtkFileReader()
{
}
