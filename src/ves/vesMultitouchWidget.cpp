/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesMultitouchWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vesMultitouchWidget.h"

// --------------------------------------------------------------------includes
#include "vtkPainter.h"

// -----------------------------------------------------------------------macro

// -----------------------------------------------------------------------cnstr
vesMultitouchWidget::vesMultitouchWidget()
{
}

// -----------------------------------------------------------------------destr
vesMultitouchWidget::~vesMultitouchWidget()
{
}

void vesMultitouchWidget::Update(float xRotation,
                                  float yRotation,
                                  float scaleFactor,
                                  float xTranslation,
                                  float yTranslation)
{
  std::cout << "**** Multitouch Widget update ****" <<std::endl;
  Translation[0] = xTranslation*1000;
  Translation[1] = yTranslation*1000;
  Translation[2] = 0;
  Rotation[0] = 0;
  Rotation[1] = 0;
  Rotation[2] = 1;
  Rotation[3] = xRotation/100;
  Scale[0] = scaleFactor;
  Scale[1] = scaleFactor;
  Scale[2] = scaleFactor;
}

bool vesMultitouchWidget::Read()
{
  return true;
}

void vesMultitouchWidget::Render(vtkPainter* render)
{

}


