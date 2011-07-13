/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vesFileReader.cpp
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vesFileReader.h"

vesFileReader::vesFileReader(std::string fileName)
{
  this->mFileName = fileName;
  this->mHasError = false;
}

vesFileReader::~vesFileReader()
{
}
