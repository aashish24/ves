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

#include "vesPVWebDataSet.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <fstream>

vesPVWebDataSet::vesPVWebDataSet() :
  m_id(0), m_part(0), m_layer(0), m_transparency(0),
  m_writePosition(0), m_bufferSize(0), m_buffer(NULL),
  m_verts(NULL), m_indices(NULL), m_colors(NULL),
  m_numberOfVerts(0), m_numberOfIndices(0), m_datasetType(0)
{
}

vesPVWebDataSet::~vesPVWebDataSet()
{
  free(this->m_buffer);
  delete [] this->m_verts;
  delete [] this->m_colors;
  delete [] this->m_indices;
}

float* vesPVWebDataSet::vertices() const
{
  return this->m_verts;
}

float* vesPVWebDataSet::normals() const
{
  size_t normalOffset = m_numberOfVerts*3;
  return this->m_verts + normalOffset;
}

short* vesPVWebDataSet::indices() const
{
  return this->m_indices;
}

unsigned char* vesPVWebDataSet::colors() const
{
  return this->m_colors;
}

const float* vesPVWebDataSet::matrix() const
{
  return this->m_matrix;
}

void vesPVWebDataSet::printSelf() const
{
  printf("verts\n");
  for (int i = 0; i < this->m_numberOfVerts; ++i) {
    float* verts = this->vertices();
    printf("%f %f %f\n", verts[i*3 + 0], verts[i*3 + 1], verts[i*3 + 2]);
  }

  if (this->m_datasetType == 'M') {
    printf("normals\n");
    for (int i = 0; i < this->m_numberOfVerts; ++i) {
      float* normals = this->normals();
      printf("%f %f %f\n", normals[i*3 + 0], normals[i*3 + 1], normals[i*3 + 2]);
    }
  }

  printf("colors\n");
  for (int i = 0; i < this->m_numberOfVerts; ++i) {
    unsigned char* colors = this->colors();
    printf("%d %d %d %d\n", colors[i*4 + 0], colors[i*4 + 1], colors[i*4 + 2], colors[i*4 + 3]);
  }

  printf("indices\n");
  for (int i = 0; i < this->m_numberOfIndices; ++i) {
    short* indices = this->indices();
    printf("%d\n", indices[i]);
  }

  printf("matrix\n");
  for (int i = 0; i < 4; ++i) {
    printf("%f %f %f %f\n", m_matrix[i*4], m_matrix[i*4+1], m_matrix[i*4+2], m_matrix[i*4+3]);
  }
}

void vesPVWebDataSet::initFromBuffer()
{

  // 'M' triangle mesh - verts, normals, colors, indices
  // 'L' lines - verts, colors, indices
  // 'P' points - verts, colors

  // dataLength(int)
  // datasetType(char)
  // numberOfVerts(int)
  // verts(float*3*numberOfVerts)
  // normals(float*3*numberOfVerts)        -  mesh only
  // color(unsigned char*4*numberOfVerts)
  // numberOfIndices(int)                  -  mesh or lines
  // indices(short*numberOfIndices)        -  mesh or lines
  // matrix(float*16)

  m_numberOfVerts = 0;
  m_numberOfIndices = 0;


  int dataLength = *reinterpret_cast<int*>(this->m_buffer + 0);
  m_datasetType = this->m_buffer[4];

  //printf("data length: %d\n", dataLength);
  //printf("type: %c\n", m_datasetType);

  if (m_datasetType != 'M' && m_datasetType != 'L' && m_datasetType != 'P') {
    std::cout << "initFromBuffer: unexpected dataset type: " << m_datasetType << std::endl;
    return;
  }

  size_t floatsPerVertex = 3;
  if (m_datasetType == 'M') {
    floatsPerVertex = 6;
  }

  m_numberOfVerts = *reinterpret_cast<int*>(this->m_buffer + sizeof(int) + sizeof(char));
  size_t vertOffset = sizeof(int)*2 + sizeof(char);
  size_t vertLength = m_numberOfVerts*sizeof(float)*floatsPerVertex;
  size_t colorOffset = vertOffset + vertLength;
  size_t colorLength = m_numberOfVerts*sizeof(unsigned char)*4;

  //printf("numberOfVerts: %d\n", m_numberOfVerts);
  //printf("vertOffset: %d\n", vertOffset);
  //printf("vertLength: %d\n", vertLength);
  //printf("colorOffset: %d\n", colorOffset);
  //printf("colorLength: %d\n", colorLength);

  m_verts = new float[m_numberOfVerts*floatsPerVertex];
  memcpy(m_verts, this->m_buffer+vertOffset, vertLength);

  m_colors = new unsigned char[m_numberOfVerts*4];
  memcpy(m_colors, this->m_buffer+colorOffset, colorLength);

  size_t pos = colorOffset + colorLength;

  if (m_datasetType == 'M' || m_datasetType == 'L') {
    m_numberOfIndices = *reinterpret_cast<int*>(this->m_buffer + pos);

    pos += sizeof(int);
    size_t indicesLength = m_numberOfIndices*sizeof(short);

    m_indices = new short[m_numberOfIndices];
    memcpy(m_indices, this->m_buffer+pos, indicesLength);
    pos += indicesLength;
  }

  memcpy(m_matrix, this->m_buffer+pos, 16*sizeof(float));
}

vesPVWebDataSet::Ptr vesPVWebDataSet::loadDataSetFromFile(const std::string& filename)
{
  vesPVWebDataSet::Ptr dataset;

  std::ifstream file(filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

  if(!file.is_open()) {
    printf("error opening file: %s\n", filename.c_str());
    return dataset;
  }

  size_t numberOfBytes = file.tellg();
  file.seekg(0, std::ios::beg);

  dataset = vesPVWebDataSet::Ptr(new vesPVWebDataSet);
  dataset->m_buffer = static_cast<char*>(malloc(numberOfBytes));

  if (!file.read(dataset->m_buffer, numberOfBytes)) {
    printf("error reading file: %s\n", filename.c_str());
    dataset.reset();
  }

  dataset->initFromBuffer();
  free(dataset->m_buffer);
  dataset->m_buffer = 0;
  return dataset;
}
