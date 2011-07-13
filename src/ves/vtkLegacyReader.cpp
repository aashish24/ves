/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkLegacyReader.cpp
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkLegacyReader.h"

vtkTriangleData* vtkLegacyReader::Read()
{  
  mHasError = false;
  
  std::ifstream fileStream(mFileName.c_str());
  if (!fileStream) {
    mHasError = true;
    return false;
  }
  
  bool hasPoints = false;
  bool hasPolygons = false;
  bool hasLines = false;
  
  vtkTriangleData* t = new vtkTriangleData();
  while (!fileStream.eof()) {
    std::string str;
    fileStream >> str;
    if (str == "BINARY") {
      mHasError = true;
      break;
    }
    else if (str == "DATASET") {
      std::string datatype;
      fileStream >> datatype;
      if (datatype != "POLYDATA") {
        mHasError = true;
        break;
      }
    }
    else if (str == "POINTS") {
      hasPoints = true;
      size_t n = 0;
      fileStream >> n >> str;
      if (n > (1 << 16)) {
        this->mHasError = true;
        this->mErrorMessage = "File too large (limit is 65,536 points).";
        break;
      }
      t->GetPoints().resize(n);
      readPoints(fileStream, &t->GetPoints()[0], n);
    }
    else if (str == "POLYGONS") {
      hasPolygons = true;
      unsigned int n = 0, e = 0;
      fileStream >> n >> e;
      readPolygons(fileStream, t->GetTriangles(), n);
    }
    else if (str == "LINES") {
      hasLines = true;
      unsigned int n = 0, e = 0;
      fileStream >> n >> e;
      readLines(fileStream, t->GetLines(), n);
    }
    else if (str == "POINT_DATA") {
      unsigned short n = 0;
      fileStream >> n;

      // make sure we read the normals and not something like scalar data
      while (!fileStream.eof() && str != "NORMALS")
      {
        fileStream >> str;
      }
      if (fileStream.eof())
      {
        mHasError = true;
        break;
      }
      
      while (!fileStream.eof() && str != "float")
      {
        fileStream >> str;
      }
      if (fileStream.eof())
      {
        mHasError = true;
        break;
      }
      
      if (n == t->GetPoints().size()) {
        readNormals(fileStream, &t->GetPoints()[0],  n);
        t->SetHasNormals(true);
      }
    }
  }
  if (!hasPoints || !(hasPolygons || hasLines))
  {
    mHasError = true;
  }
  fileStream.close();
  return t;
}

void vtkLegacyReader::readPoints(std::ifstream &file, vtkVertex3f *v, int n)
{
	for (int i = 0; i < n; ++i, ++v){
		file >> v->point[0] >> v->point[1] >> v->point[2];
	}
}

void vtkLegacyReader::readNormals(std::ifstream &file, vtkVertex3f *v, int n)
{
  for (int i = 0; i < n; ++i, ++v)
    file >> v->normal[0] >> v->normal[1] >> v->normal[2];
}

void vtkLegacyReader::readPolygons(std::ifstream &file, std::vector<vesVector3us>& triangleCells, int numPolygons)
{
  unsigned short numVertices = 0;
  unsigned short numPolygonsRead = 0;
  for (int i = 0; i < numPolygons; ++i) 
  {
    file >> numVertices;
    if (numVertices == 3) 
    {
      vesVector3us indices;
      file >> indices[0] >> indices[1] >> indices[2];
      triangleCells.push_back(indices);
      numPolygonsRead++;
    }
    else if (numVertices == 4)
    {
      vesVector3us indices1;
      vesVector3us indices2;
      file >> indices1[0] >> indices1[1] >> indices1[2] >> indices2[0];
      indices2[1] = indices1[0];
      indices2[2] = indices1[2];
      triangleCells.push_back(indices1);
      triangleCells.push_back(indices2);
      numPolygonsRead++;
    }
  }
  if (numPolygonsRead == 0)
  {
    mHasError = true;
  }
}

void vtkLegacyReader::readLines(std::ifstream &file, std::vector<vesVector2us>& lineCells, int numLines)
{
  unsigned short numVertices = 0;
  unsigned short numLinesRead = 0;
  for (int i = 0; i < numLines; ++i) 
  {
    file >> numVertices;
    for (int j = 0; j < numVertices-1; ++j)
    {
      vesVector2us indices;
      file >> indices[0] >> indices[1];
      lineCells.push_back(indices);
      ++numLinesRead;
    }
  }
  if (numLinesRead == 0)
  {
    mHasError = true;
  }
}


