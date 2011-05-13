/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSTLReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkSTLReader - read ASCII or binary stereo lithography files
// .SECTION Description
// vtkSTLReader is a source object that reads ASCII or binary stereo 
// lithography files (.stl files). The FileName must be specified to
// vtkSTLReader. The object automatically detects whether the file is
// ASCII or binary.

// .SECTION Caveats
// Binary files written on one system may not be readable on other systems.
// vtkSTLWriter uses VAX or PC byte ordering and swaps bytes on other systems.

#ifndef __vtkSTLReader_h
#define __vtkSTLReader_h

#include "vtkFileReader.h"

#include <string>

class vtkSTLReader : public vtkFileReader
{
public:
  vtkSTLReader(std::string& filename) : vtkFileReader(filename) {};
  ~vtkSTLReader() {};
  
  // Description:
  // Construct object with merging set to true.
  virtual vtkTriangleData* Read();

protected:
  int ReadBinarySTL(FILE *fp, vtkTriangleData* t);
  int ReadASCIISTL(FILE *fp, vtkTriangleData* t);
  int GetSTLFileType(std::string filename);
};

#endif
