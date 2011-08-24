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
