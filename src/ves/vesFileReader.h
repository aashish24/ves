/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vesFileReader.h

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#ifndef vesFileReader_h
#define vesFileReader_h

#include "vesGMTL.h"
#include "vesSetGet.h"
#include "vesTriangleData.h"

#include <vector>
#include <string>
#include <fstream>

class vesFileReader
{
public:
  vesTypeMacro(vesFileReader);

  vesFileReader(std::string filename);
  ~vesFileReader();
  virtual vesTriangleData* Read() = 0;
  virtual bool HasError() { return this->mHasError; }
  virtual std::string GetErrorMessage() { return this->mErrorMessage; }
protected:
  std::string mFileName;
  bool mHasError;
  std::string mErrorMessage;
};

#endif
