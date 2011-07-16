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
#include "vtkPolyDataReader.h"


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


- (vesTriangleData*)readData:(NSString*)fpath
{
  std::string str = [fpath UTF8String];
  vesTriangleData* newData = 0;

  if (hasEnding(str, "vtk"))
    {
    vtkPolyDataReader* reader = vtkPolyDataReader::New();
    reader->SetFileName(str.c_str());
    reader->Update();
    newData = vesPolyDataToTriangleData::Convert(reader->GetOutput());
    reader->Delete();
    }
  else if(hasEnding(str, "stl"))
    {
    vesSTLReader* reader = new vesSTLReader(str);
    newData = reader->Read();
    if (!newData || reader->HasError())
      {
      NSString *err = @"Could not read the file specified";
      if (reader->GetErrorMessage().size() > 0)
        {
        err = [NSString stringWithUTF8String:reader->GetErrorMessage().c_str()];
        }
      delete newData;
      delete reader;
      self.errorTitle = NSLocalizedStringFromTable(@"Could not read file", @"Localized", nil);
      self.errorMessage = NSLocalizedStringFromTable(err, @"Localized", nil);
      return 0;
      }
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