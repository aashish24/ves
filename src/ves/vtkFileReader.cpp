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

vtkFileReader::vtkFileReader()
  : m_numPoints(0), m_points(0), m_numTriangles(0), m_triangles(0)
{
}

void vtkFileReader::readFile(const std::string &fileName)
{
  std::ifstream fileStream(fileName.c_str());
  if (!fileStream) {
    return;
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
      m_triangles = new vtkVector3us[n];
      m_numTriangles = n;
      readTriangles(fileStream, m_triangles, n);
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

}

vtkFileReader::~vtkFileReader()
{
  delete[] m_points;
  delete[] m_triangles;
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
			if(v->point[0] < min[1]) min[1] = v->point[1];
			if(v->point[0] < min[2]) min[2] = v->point[2];
			if(v->point[0] > max[0]) max[0] = v->point[0];
			if(v->point[0] > max[1]) max[1] = v->point[1];
			if(v->point[0] > max[2]) max[2] = v->point[2];
		}
	}
	center[0] = (min[0]+max[0])/2;
	center[1] = (min[1]+max[1])/2;
	center[2] = (min[2]+max[2])/2;
        float x = max[0]-min[0];
        float y = max[1]-min[1];
        float z = max[2]-min[2];
        radius = sqrt(x*x+y*y+z*z);
}

void vtkFileReader::readNormals(std::ifstream &file, Vertex3f *v, int n)
{
  for (int i = 0; i < n; ++i, ++v)
    file >> v->normal[0] >> v->normal[1] >> v->normal[2];
}

void vtkFileReader::readTriangles(std::ifstream &file, vtkVector3us *indices, int n)
{
  unsigned short tri = 0;
  for (int i = 0; i < n; ++i) {
    file >> tri;
    if (tri == 3) {
      file >> (*indices)[0] >> (*indices)[1] >> (*indices)[2];
      ++indices;
    }
  }
}
