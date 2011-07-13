/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesMultitouchWidget.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesMultitouchWidget -
// .SECTION Description
// vesMultitouchWidget

#ifndef __vesMultitouchWidget_h
#define __vesMultitouchWidget_h
// --------------------------------------------------------------------includes
#include "vesMultitouchSensorNode.h"

// -----------------------------------------------------------------pre-defines
class Painter;

// -----------------------------------------------------------------------class
class vesMultitouchWidget : public vesMultitouchSensorNode
{
public:
  // ............................................................public-methods
  vesMultitouchWidget();
  ~vesMultitouchWidget();
  bool Read();
  void Render(Painter* render);
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

#endif // __vesMultitouchWidget_h
