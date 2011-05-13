/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkLegacyReader.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vtkLegacyReader_H
#define vtkLegacyReader_H

#include "vtkFileReader.h"

// Small struct packing a point and normal together in a vertex
// Memory layout is 3 floats for a point followed by 3 floats for a normal
struct Vertex3f
{
  vtkVector3f point;
  vtkVector3f normal;
};

#include <string>
#include <fstream>

class vtkLegacyReader : public vtkFileReader
{
public:
  vtkLegacyReader(std::string filename) : vtkFileReader(filename) {}
  ~vtkLegacyReader() {}

  virtual vtkTriangleData* Read();

private:
  void readPoints(std::ifstream &file, vtkVertex3f *v, int n);
  void readNormals(std::ifstream &file, vtkVertex3f *v, int n);
  void readPolygons(std::ifstream &file, std::vector<vtkVector3us>& triangleCells, int numPolygons);
};

#endif
