/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkSTLReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkSTLReader.h"

#include "vtkByteSwap.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <ctype.h>
#include <iostream>
#include <vector>
#include <map>

#define VTK_ASCII 0
#define VTK_BINARY 1

#define vtkErrorMacro(x) std::cerr << "ERROR " x
#define vtkWarningMacro(x) std::cerr << "WARNING " x
#define vtkDebugMacro(x)

struct SystemTools
{
  /**
   * Attempt to detect and return the type of a file.
   * Up to 'length' bytes are read from the file, if more than 'percent_bin' %
   * of the bytes are non-textual elements, the file is considered binary,
   * otherwise textual. Textual elements are bytes in the ASCII [0x20, 0x7E]
   * range, but also \n, \r, \t.
   * The algorithm is simplistic, and should probably check for usual file
   * extensions, 'magic' signature, unicode, etc.
   */
  enum FileTypeEnum 
  { 
    FileTypeUnknown,
    FileTypeBinary,
    FileTypeText
  };
  static FileTypeEnum DetectFileType(const char *filename,
                                     unsigned long length = 256, 
                                     double percent_bin = 0.05)
  {
    if (!filename || percent_bin < 0)
    {
      return SystemTools::FileTypeUnknown;
    }
    
    FILE *fp;
    fp = fopen(filename, "rb");
    if (!fp)
    {
      return SystemTools::FileTypeUnknown;
    }
    
    // Allocate buffer and read bytes
    
    unsigned char *buffer = new unsigned char [length];
    size_t read_length = fread(buffer, 1, length, fp);
    fclose(fp);
    if (read_length == 0)
    {
      return SystemTools::FileTypeUnknown;
    }
    
    // Loop over contents and count
    
    size_t text_count = 0;
    
    const unsigned char *ptr = buffer;
    const unsigned char *buffer_end = buffer + read_length;
    
    while (ptr != buffer_end)
    {
      if ((*ptr >= 0x20 && *ptr <= 0x7F) || 
          *ptr == '\n' ||
          *ptr == '\r' ||
          *ptr == '\t')
      {
        text_count++;
      }
      ptr++;
    }
    
    delete [] buffer;
    
    double current_percent_bin =  
    (static_cast<double>(read_length - text_count) /
     static_cast<double>(read_length));
    
    if (current_percent_bin >= percent_bin)
    {
      return SystemTools::FileTypeBinary;
    }
    
    return SystemTools::FileTypeText;
  }
  
  static unsigned long FileLength(const char* filename)
  {
    struct stat fs;
    if (stat(filename, &fs) != 0) 
    {
      return 0;
    }
    else
    {
      return static_cast<unsigned long>(fs.st_size);
    }
  }
  
};

class vtkPoints
{
public:
  static vtkPoints* New() { return new vtkPoints(); }
  vtkPoints() {}
  ~vtkPoints() {}
  void Allocate(int, int) { }
  int InsertNextPoint(float* pt)
  {
    s.push_back(pt[0]);
    s.push_back(pt[1]);
    s.push_back(pt[2]);
    return s.size()/3;
  }
private:
  std::vector<float> s;
};

class vtkCellArray
{
public:
  static vtkCellArray* New() { return new vtkCellArray(); }
  vtkCellArray() {}
  ~vtkCellArray() {}
  void Allocate(int, int) { }
  int InsertNextCell(int len, int* cells)
  {
    s.push_back(std::vector<int>(cells, cells+len));
    return s.size();
  }
private:
  std::vector<std::vector<int> > s;
};

vtkTriangleData* vtkSTLReader::Read()
{
  FILE *fp;
  vtkPoints *newPts;
  vtkCellArray *newPolys;

  // Initialize
  //
  if ((fp = fopen(this->mFileName.c_str(), "r")) == NULL)
    {
    vtkErrorMacro(<< "File " << this->mFileName << " not found");
    return 0;
    }

  vtkTriangleData* t = new vtkTriangleData();
  
  newPts = vtkPoints::New();
  newPolys = vtkCellArray::New();

  // Depending upon file type, read differently
  //
  if ( vtkSTLReader::GetSTLFileType(this->mFileName) == VTK_ASCII )
    {
    newPts->Allocate(5000,10000);
    newPolys->Allocate(10000,20000);
    this->ReadASCIISTL(fp,t);
    }
  else
    {
    fclose(fp);
    fp = fopen(this->mFileName.c_str(), "rb");
    this->ReadBinarySTL(fp,t);
    }

  vtkDebugMacro(<< "Read: "
                << newPts->GetNumberOfPoints() << " points, "
                << newPolys->GetNumberOfCells() << " triangles");

  fclose(fp);
  return t;
}

struct VectorCompare
{
  bool operator()(const vtkVector3f& a, const vtkVector3f& b)
  {
    for (int i = 0; i < 3; ++i)
      {
      if (a[i] != b[i])
        {
        return a[i] < b[i];
        }
      }
    return false;
  }
};

int vtkSTLReader::ReadBinarySTL(FILE *fp, vtkTriangleData* t)
{
  int i, numTris;
  vtkVector3us pts;
  unsigned long   ulint;
  unsigned short  ibuff2;
  char    header[81];
  typedef struct  {float  n[3], v1[3], v2[3], v3[3];} facet_t;
  facet_t facet;

  vtkDebugMacro(<< " Reading BINARY STL file");

  //  File is read to obtain raw information as well as bounding box
  //
  fread (header, 1, 80, fp);
  fread (&ulint, 1, 4, fp);
  vtkByteSwap::Swap4LE(&ulint);

  // Many .stl files contain bogus count.  Hence we will ignore and read 
  //   until end of file.
  //
  if ( (numTris = (int) ulint) <= 0 )
    {
    vtkDebugMacro(<< "Bad binary count: attempting to correct (" 
    << numTris << ")");
    }

  // Verify the numTris with the length of the file
  unsigned long ulFileLength = SystemTools::FileLength(this->mFileName.c_str());
  ulFileLength -= (80 + 4); // 80 byte - header, 4 byte - tringle count
  ulFileLength /= 50;       // 50 byte - twelve 32-bit-floating point numbers + 2 byte for attribute byte count

  if (numTris < static_cast<int>(ulFileLength))
    numTris = static_cast<int>(ulFileLength);

  // Simple map for merging points (note that locations must match exactly)
  std::map<vtkVector3f, int, VectorCompare> locations;

  for ( i=0; fread(&facet,48,1,fp) > 0; i++ )
    {
    fread(&ibuff2,2,1,fp); //read extra junk

    vtkByteSwap::Swap4LE (facet.n);
    vtkByteSwap::Swap4LE (facet.n+1);
    vtkByteSwap::Swap4LE (facet.n+2);

    vtkByteSwap::Swap4LE (facet.v1);
    vtkByteSwap::Swap4LE (facet.v1+1);
    vtkByteSwap::Swap4LE (facet.v1+2);
    vtkVertex3f v;
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = facet.v1[j];
      v.normal[j] = facet.n[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[0] = locations[v.point];
      }
    else
      {
      pts[0] = t->GetPoints().size();
      //locations[v.point] = pts[0];
      t->GetPoints().push_back(v);
      }

    vtkByteSwap::Swap4LE (facet.v2);
    vtkByteSwap::Swap4LE (facet.v2+1);
    vtkByteSwap::Swap4LE (facet.v2+2);
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = facet.v2[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[1] = locations[v.point];
      }
    else
      {
      pts[1] = t->GetPoints().size();
      //locations[v.point] = pts[1];
      t->GetPoints().push_back(v);
      }
    
    vtkByteSwap::Swap4LE (facet.v3);
    vtkByteSwap::Swap4LE (facet.v3+1);
    vtkByteSwap::Swap4LE (facet.v3+2);
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = facet.v3[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[2] = locations[v.point];
      }
    else
      {
      pts[2] = t->GetPoints().size();
      //locations[v.point] = pts[2];
      t->GetPoints().push_back(v);
      }
    
    t->GetTriangles().push_back(pts);
    }
  t->SetHasNormals(true);
  std::cerr << "Points: " << t->GetPoints().size() << std::endl;
  std::cerr << "Triangles: " << t->GetTriangles().size() << std::endl;
  return 0;
}

int vtkSTLReader::ReadASCIISTL(FILE *fp, vtkTriangleData* t)
{
  char line[256];
  float x[3];
  vtkVector3us pts;
  int done;
  int currentSolid = 0;

  vtkDebugMacro(<< " Reading ASCII STL file");

  //  Ingest header and junk to get to first vertex
  //
  fgets (line, 255, fp);

  done = (fscanf(fp,"%s %*s %f %f %f\n", line, x, x+1, x+2)==EOF);
  if ((strcmp(line, "COLOR") == 0) || (strcmp(line, "color") == 0))
    {
    done = (fscanf(fp,"%s %*s %f %f %f\n", line, x, x+1, x+2)==EOF);
    }

  // Simple map for merging points (note that locations must match exactly)
  std::map<vtkVector3f, int, VectorCompare> locations;
  
  //  Go into loop, reading  facet normal and vertices
  while (!done)
    {
    fgets (line, 255, fp);
    fscanf (fp, "%*s %f %f %f\n", x,x+1,x+2);
    vtkVertex3f v;
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = x[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[0] = locations[v.point];
      }
    else
      {
      pts[0] = t->GetPoints().size();
      //locations[v.point] = pts[0];
      t->GetPoints().push_back(v);
      }
    fscanf (fp, "%*s %f %f %f\n", x,x+1,x+2);
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = x[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[1] = locations[v.point];
      }
    else
      {
      pts[1] = t->GetPoints().size();
      //locations[v.point] = pts[1];
      t->GetPoints().push_back(v);
      }
    fscanf (fp, "%*s %f %f %f\n", x,x+1,x+2);
    for (int j = 0; j < 3; ++j)
      {
      v.point[j] = x[j];
      }
    if (locations.find(v.point) != locations.end())
      {
      pts[2] = locations[v.point];
      }
    else
      {
      pts[2] = t->GetPoints().size();
      //locations[v.point] = pts[2];
      t->GetPoints().push_back(v);
      }
    fgets (line, 255, fp); // end loop
    fgets (line, 255, fp); // end facet

    t->GetTriangles().push_back(pts);

    done = (fscanf(fp,"%s", line)==EOF);
    if ((strcmp(line, "ENDSOLID") == 0) || (strcmp(line, "endsolid") == 0)) 
      {
      currentSolid++;
      fgets(line, 255, fp);
      done = feof(fp);
      while ((strstr(line, "SOLID") == 0) && (strstr(line, "solid") == 0) && !done) 
        {
        fgets(line, 255, fp);
        done = feof(fp);
        }

      done = (fscanf(fp,"%s", line)==EOF);
      if ((strstr(line, "COLOR") == 0) || (strstr(line, "color") == 0))
        {
        done = (fscanf(fp,"%f %f %f\n", x,x+1,x+2)==EOF);
        done = (fscanf(fp,"%s", line)==EOF);
        }
      }
    if (!done)
      {
      done = (fscanf(fp,"%*s %f %f %f\n", x, x+1, x+2)==EOF);
      }
    }
  return 0;
}

int vtkSTLReader::GetSTLFileType(std::string filename)
{
  int type;
  SystemTools::FileTypeEnum ft =
    SystemTools::DetectFileType(filename.c_str());
  switch(ft)
    {
  case SystemTools::FileTypeBinary:
    type = VTK_BINARY;
    break;
  case SystemTools::FileTypeText:
    type = VTK_ASCII;
    break;
  case SystemTools::FileTypeUnknown:
    vtkWarningMacro( "File type not recognized attempting binary" );
    type = VTK_BINARY;
    break;
  default:
    vtkErrorMacro( "Case not handled" );
    type = VTK_BINARY; // should not happen
    }
  return type;
}
