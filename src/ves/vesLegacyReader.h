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

#ifndef vesLegacyReader_H
#define vesLegacyReader_H

#include "vesFileReader.h"

// VES includes
#include "vesSetGet.h"

// Small struct packing a point and normal together in a vertex
// Memory layout is 3 floats for a point followed by 3 floats for a normal
struct Vertex3f
{
  vesVector3f point;
  vesVector3f normal;
};

#include <string>
#include <fstream>

class vesLegacyReader : public vesFileReader
{
public:
  vesTypeMacro(vesLegacyReader);

  vesLegacyReader(std::string filename) : vesFileReader(filename) {}
  ~vesLegacyReader() {}

  virtual vesTriangleData* Read();

private:
  void readPoints(std::ifstream &file, vtkVertex3f *v, int n);
  void readNormals(std::ifstream &file, vtkVertex3f *v, int n);
  void readPolygons(std::ifstream &file, std::vector<vesVector3us>& triangleCells, int numPolygons);
  void readLines(std::ifstream &file, std::vector<vesVector2us>& lineCells, int numLines);
};

#endif
