/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMultitouchWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMultitouchWidget -
// .SECTION Description
// vtkMultitouchWidget

#ifndef __vtkMultitouchWidget_h
#define __vtkMultitouchWidget_h
// --------------------------------------------------------------------includes
#include "vtkMultitouchSensorNode.h"

// -----------------------------------------------------------------pre-defines
class vtkPainter;

// -----------------------------------------------------------------------class
class vtkMultitouchWidget : public vtkMultitouchSensorNode
{
public:
  // ............................................................public-methods
  vtkMultitouchWidget();
  ~vtkMultitouchWidget();
  bool Read();
  void Render(vtkPainter* render);
  void Update(float xRotation,
              float yRotation,
              float scaleFactor,
              float xTranslation,
              float yTranslation);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX

};

#endif // __vtkMultitouchWidget_h
