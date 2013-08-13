/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkPCDReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkPCDReader.h"

#include "vtkErrorCode.h"
#include "vtkPolyData.h"
#include "vtkTransform.h"
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkObjectFactory.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkSmartPointer.h"
#include "vtkNew.h"

#include "vtkPCLConversions.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPCDReader);

//----------------------------------------------------------------------------
vtkPCDReader::vtkPCDReader()
{
  this->FileName = 0;
  this->SetNumberOfInputPorts(0);
  this->SetNumberOfOutputPorts(1);
}

//----------------------------------------------------------------------------
vtkPCDReader::~vtkPCDReader()
{
  this->SetFileName(NULL);
}

//----------------------------------------------------------------------------
vtkTransform* vtkPCDReader::GetTransform()
{
  return this->Transform;
}

//----------------------------------------------------------------------------
int vtkPCDReader::RequestData(
  vtkInformation *vtkNotUsed(request),
  vtkInformationVector **inputVector,
  vtkInformationVector *outputVector)
{
  vtkInformation *outInfo = outputVector->GetInformationObject(0);
  vtkDataSet *output = vtkDataSet::SafeDownCast(outInfo->Get(vtkDataObject::DATA_OBJECT()));

  if (!this->GetFileName())
    {
    this->SetErrorCode(vtkErrorCode::NoFileNameError);
    vtkErrorMacro("Filename is not set");
    return 0;
    }

  this->Transform = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkPolyData> polyData = vtkPCLConversions::PolyDataFromPCDFile(this->GetFileName(), this->Transform);

  if (!polyData)
    {
    this->SetErrorCode(vtkErrorCode::FileFormatError);
    vtkErrorMacro("Failed to read pcd file: " << this->GetFileName());
    return 0;
    }

  output->ShallowCopy(polyData);
  return 1;
}

//----------------------------------------------------------------------------
void vtkPCDReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
