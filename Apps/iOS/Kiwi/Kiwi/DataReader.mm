//
//  DataReader.mm
//  kiwi
//
//  Created by pat on 7/9/11.
//  Copyright 2011 Kitware. All rights reserved.
//

#import "DataReader.h"

#include "vesTriangleData.h"
#include "vesSTLReader.h"
#include "vesPolyDataToTriangleData.h"

#include "vtkSmartPointer.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"
#include "vtkOBJReader.h"
#include "vtkSTLReader.h"
#include "vtkTriangleFilter.h"
#include "vtkErrorCode.h"


@implementation DataReader

@synthesize errorTitle;
@synthesize errorMessage;


bool hasEnding(std::string const &fullString, std::string const &ending)
{
  if (fullString.length() > ending.length())
    {
    return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    }
  else
    {
    return false;
    }
}


-(void)setMaximumNumberOfPointsErrorMessage
{
  self.errorTitle = NSLocalizedStringFromTable(@"Could not read file", @"Localized", nil);
  self.errorMessage = NSLocalizedStringFromTable(@"The file contains more than the supported maximum number of points.", @"Localized", nil);
}


// Updates the given algorithm.  Returns nil if the update is successful.
// If there is an error, returns the error string.
-(NSString*)safelyUpdateAlgorithm:(vtkAlgorithm*)algorithm
{
  algorithm->Update();
  unsigned long errorCode = algorithm->GetErrorCode();
  if (errorCode == vtkErrorCode::NoError)
    {
    return nil;
    }
  else
    {
    const char* errorString = vtkErrorCode::GetStringFromErrorCode(errorCode);
    if (!errorString || !errorString[0])
      {
      return @"Could not read the file specified";
      }
    return [NSString stringWithUTF8String:errorString];
    }
}


-(BOOL)updateAlgorithmOrSetErrorString:(vtkAlgorithm*)algorithm
{
  NSString* errorString = [self safelyUpdateAlgorithm:algorithm];
  if (errorString != nil)
    {
    self.errorTitle = NSLocalizedStringFromTable(@"Could not read file", @"Localized", nil);
    self.errorMessage = NSLocalizedStringFromTable(errorString, @"Localized", nil);
    return FALSE;
    }
  return TRUE;
}


// It is mandatory that the caller passes a vtkAlgorithm that produces vtkPolyData.
-(vesTriangleData*) dataFromPolyDataReader:(vtkAlgorithm*)reader
{
  if (![self updateAlgorithmOrSetErrorString:reader])
    {
    return 0;
    }

  vtkPolyData* readerOutput = vtkPolyData::SafeDownCast(reader->GetOutputDataObject(0));
  assert(readerOutput);

  vtkIdType maximumNumberOfPoints = 65536;
  if (readerOutput->GetNumberOfPoints() > maximumNumberOfPoints)
    {
    [self setMaximumNumberOfPointsErrorMessage];
    return 0;
    }


  // Always use triangle filter for now.  This will ensure that models containing
  // polygons other than tris and quads will be rendered correctly.
  BOOL useTriangleFilter = TRUE;
  if (useTriangleFilter)
    {
    vtkSmartPointer<vtkTriangleFilter> triangleFilter = vtkSmartPointer<vtkTriangleFilter>::New();
    triangleFilter->PassLinesOn();
    triangleFilter->SetInputConnection(reader->GetOutputPort());
    if (![self updateAlgorithmOrSetErrorString:triangleFilter])
      {
      return 0;
      }
    return vesPolyDataToTriangleData::Convert(triangleFilter->GetOutput());
    }
  else
    {
    return vesPolyDataToTriangleData::Convert(readerOutput);
    }
}


- (vesTriangleData*)readData:(NSString*)fpath
{
  std::string str = [fpath UTF8String];
  vesTriangleData* newData = 0;

  if (hasEnding(str, "vtk"))
    {
    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
    reader->SetFileName(str.c_str());
    return [self dataFromPolyDataReader:reader];
    }
  if (hasEnding(str, "vtp"))
    {
    vtkSmartPointer<vtkXMLPolyDataReader> reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    reader->SetFileName(str.c_str());
    return [self dataFromPolyDataReader:reader];
    }
  else if (hasEnding(str, "obj"))
    {
    vtkSmartPointer<vtkOBJReader> reader = vtkSmartPointer<vtkOBJReader>::New();
    reader->SetFileName(str.c_str());
    return [self dataFromPolyDataReader:reader];
    }
  else if (hasEnding(str, "stl"))
    {
    vtkSmartPointer<vtkSTLReader> reader = vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName(str.c_str());
    return [self dataFromPolyDataReader:reader];
    }
  else
    {
    self.errorTitle = NSLocalizedStringFromTable(@"Unsupported file format", @"Localized", nil);
    self.errorMessage = NSLocalizedStringFromTable(@"Cannot read files of this format", @"Localized", nil);
    return 0;
    }

  return newData;
}

@end