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

#ifndef ANDROID
#import <QuartzCore/QuartzCore.h>
#endif

#include "vesGMTL.h"

#define GMTL_CAMERA 0

// -----------------------------------------------------------------pre-defines
class Painter;

// -----------------------------------------------------------------------class
class vesMultitouchCamera : public vesMultitouchSensorNode
{
public:
  // ............................................................public-methods
  vesMultitouchCamera();
  ~vesMultitouchCamera();

  bool Read();
  void Render(Painter* render);
  void SetWidthHeight(const unsigned int width,
                      const unsigned int height);
  vesMatrix4x4f GetMatrix();
  vesMatrix4x4f GetMatrixGMTL();
  void Reset();

  // Description:
  // Rotate the camera matrix by angle radians about the axis [x,y,z]
  void RotateAngleAxis(double angle, double x, double y, double z);

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
#ifndef ANDROID
  CATransform3D currentCalculatedMatrix, testMatrix;
#endif

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX


};

#endif // __vesMultitouchCamera_h
