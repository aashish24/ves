/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkFileReader.cpp
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkFileReader.h"

vtkFileReader::vtkFileReader(std::string fileName)
  : m_numPoints(0), m_points(0)
{
  this->mFileName = fileName;
}

bool vtkFileReader::read()
{
  std::ifstream fileStream(mFileName.c_str());
  if (!fileStream) {
    return false;
  }

  while (!fileStream.eof()) {
    std::string str;
    fileStream >> str;

    if (str == "POINTS") {
      unsigned short n = 0;
      fileStream >> n >> str;
      m_points = new Vertex3f[n];
      m_numPoints = n;
      readPoints(fileStream, m_points, n);
    }
    else if (str == "POLYGONS") {
      unsigned int n = 0, e = 0;
      fileStream >> n >> e;
      readPolygons(fileStream, m_triangles, n);
    }
    else if (str == "POINT_DATA") {
      unsigned short n = 0;
      fileStream >> n;
      while (str != "float")
        fileStream >> str;
      if (n == m_numPoints && m_points)
        readNormals(fileStream, m_points,  n);
    }
  }
  return true;
}

vtkFileReader::~vtkFileReader()
{
  
  delete[] m_points;
}

void vtkFileReader::readPoints(std::ifstream &file, Vertex3f *v, int n)
{
	for (int i = 0; i < n; ++i, ++v){
		file >> v->point[0] >> v->point[1] >> v->point[2];
		if(0==i){
			min[0] = max[0] = v->point[0];
			min[1] = max[1] = v->point[1];
			min[2] = max[2] = v->point[2];
		}else {
			if(v->point[0] < min[0]) min[0] = v->point[0];
			if(v->point[1] < min[1]) min[1] = v->point[1];
			if(v->point[2] < min[2]) min[2] = v->point[2];
			if(v->point[0] > max[0]) max[0] = v->point[0];
			if(v->point[1] > max[1]) max[1] = v->point[1];
			if(v->point[2] > max[2]) max[2] = v->point[2];
		}
	}
}

//void vtkFileReader::ComputeMinMax(vtkPoint3f point)
//{
//}


void vtkFileReader::readNormals(std::ifstream &file, Vertex3f *v, int n)
{
  for (int i = 0; i < n; ++i, ++v)
    file >> v->normal[0] >> v->normal[1] >> v->normal[2];
}

void vtkFileReader::readPolygons(std::ifstream &file, std::vector<vtkVector3us>& triangleCells, int numPolygons)
{
  unsigned short numVertices = 0;
  for (int i = 0; i < numPolygons; ++i) 
  {
    file >> numVertices;
    if (numVertices == 3) 
    {
      vtkVector3us indices;
      file >> indices[0] >> indices[1] >> indices[2];
      triangleCells.push_back(indices);
    }
    else if (numVertices == 4)
    {
      vtkVector3us indices1;
      vtkVector3us indices2;
      file >> indices1[0] >> indices1[1] >> indices1[2] >> indices2[0];
      indices2[1] = indices1[0];
      indices2[2] = indices1[2];
      triangleCells.push_back(indices1);
      triangleCells.push_back(indices2);
    }
  }
}

