/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMultitouchWidget.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkMultitouchWidget.h"

// --------------------------------------------------------------------includes
#include "vtkPainter.h"

// -----------------------------------------------------------------------macro

// -----------------------------------------------------------------------cnstr
vtkMultitouchWidget::vtkMultitouchWidget()
{
}

// -----------------------------------------------------------------------destr
vtkMultitouchWidget::~vtkMultitouchWidget()
{
}

void vtkMultitouchWidget::Update(float xRotation,
                                  float yRotation,
                                  float scaleFactor,
                                  float xTranslation,
                                  float yTranslation)
{
  std::cout << "**** Multitouch Widget update ****" <<std::endl;
  Translation[0] = xTranslation*10000;
  Translation[1] = yTranslation*10000;
  Translation[2] = 0;
}

bool vtkMultitouchWidget::Read()
{
  return true;
}

void vtkMultitouchWidget::Render(vtkPainter* render)
{

}


