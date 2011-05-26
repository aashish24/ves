/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkFileReader.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vtkFileReader_h
#define vtkFileReader_h

#include "vtkGMTL.h"
#include "vtkTriangleData.h"

#include <vector>
#include <string>
#include <fstream>

class vtkFileReader
{
public:
  vtkFileReader(std::string filename);
  ~vtkFileReader();
  virtual vtkTriangleData* Read() = 0;
  virtual bool HasError() { return this->mHasError; }
  virtual std::string GetErrorMessage() { return this->mErrorMessage; }
protected:
  std::string mFileName;
  bool mHasError;
  std::string mErrorMessage;
};

#endif
