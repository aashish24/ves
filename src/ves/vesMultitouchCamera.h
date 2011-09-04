/*========================================================================
  VES --- VTK OpenGL ES Rendering Toolkit

      http://www.kitware.com/ves

  Copyright 2011 Kitware, Inc.

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
 ========================================================================*/
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

  bool read();
  void render(Painter* render);
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
