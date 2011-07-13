/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vesLegacyReader.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vesLegacyReader_H
#define vesLegacyReader_H

#include "vesFileReader.h"

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
