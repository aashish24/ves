/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesSTLReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesSTLReader - read ASCII or binary stereo lithography files
// .SECTION Description
// vesSTLReader is a source object that reads ASCII or binary stereo 
// lithography files (.stl files). The FileName must be specified to
// vesSTLReader. The object automatically detects whether the file is
// ASCII or binary.

// .SECTION Caveats
// Binary files written on one system may not be readable on other systems.
// vtkSTLWriter uses VAX or PC byte ordering and swaps bytes on other systems.

#ifndef __vesSTLReader_h
#define __vesSTLReader_h

#include "vesFileReader.h"

#include <string>

class vesSTLReader : public vesFileReader
{
public:
  vesSTLReader(std::string& filename) : vesFileReader(filename) {};
  ~vesSTLReader() {};
  
  // Description:
  // Construct object with merging set to true.
  virtual vesTriangleData* Read();

protected:
  int ReadBinarySTL(FILE *fp, vesTriangleData* t);
  int ReadASCIISTL(FILE *fp, vesTriangleData* t);
  int GetSTLFileType(std::string filename);
};

#endif
