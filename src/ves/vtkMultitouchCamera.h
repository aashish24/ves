/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkMultitouchCamera.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkMultitouchCamera -
// .SECTION Description
// vtkMultitouchCamera

#ifndef __vtkMultitouchCamera_h
#define __vtkMultitouchCamera_h
// --------------------------------------------------------------------includes
#include "vtkMultitouchSensorNode.h"
#import <QuartzCore/QuartzCore.h>
#include "vtkGMTL.h"

#define GMTL_CAMERA 0

// -----------------------------------------------------------------pre-defines
class vtkPainter;

// -----------------------------------------------------------------------class
class vtkMultitouchCamera : public vtkMultitouchSensorNode
{
public:
  // ............................................................public-methods
  vtkMultitouchCamera();
  ~vtkMultitouchCamera();

  bool Read();
  void Render(vtkPainter* render);
  void SetWidthHeight(const unsigned int width,
                      const unsigned int height);
  vtkMatrix4x4f GetMatrix();
  vtkMatrix4x4f GetMatrixGMTL();
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
  vtkMatrix4x4f _matrix;
  CATransform3D currentCalculatedMatrix, testMatrix;

protected:
//BTX
  // .......................................................................BTX

private:

//ETX
  // .......................................................................ETX


};

#endif // __vtkMultitouchCamera_h
