/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesMultitouchCamera.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesMultitouchCamera -
// .SECTION Description
// vesMultitouchCamera

#ifndef __vesMultitouchCamera_h
#define __vesMultitouchCamera_h
// --------------------------------------------------------------------includes
#include "vesMultitouchSensorNode.h"
#import <QuartzCore/QuartzCore.h>
#include "vesGMTL.h"

#define GMTL_CAMERA 0

// -----------------------------------------------------------------pre-defines
class vtkPainter;

// -----------------------------------------------------------------------class
class vesMultitouchCamera : public vesMultitouchSensorNode
{
public:
  // ............................................................public-methods
  vesMultitouchCamera();
  ~vesMultitouchCamera();

  bool Read();
  void Render(vtkPainter* render);
  void SetWidthHeight(const unsigned int width,
                      const unsigned int height);
  vesMatrix4x4f GetMatrix();
  vesMatrix4x4f GetMatrixGMTL();
  void Reset();

  void printCurrentCalculatedMatrix();
  void printGMTLMatrix();
  void UpdateMatrix(const float xRotation,
                    const float yRotation,
                    float scaleFactor,
                    float xTranslation,
                    float yTranslation);
  void UpdateMatrixGMTL(const float xRotation,
                    const float yRotation,
                    float scaleFactor,
                    float xTranslation,
                    float yTranslation);
protected:
  // ...........................................................protected-ivars
  
  unsigned int _width,_height;
  vesMatrix4x4f _matrix;
  CATransform3D currentCalculatedMatrix, testMatrix;

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX


};

#endif // __vesMultitouchCamera_h
