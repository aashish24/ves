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

#include "vesMultitouchWidget.h"

vesMultitouchWidget::vesMultitouchWidget()
{
}


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

bool vesMultitouchWidget::read()
{
  return true;
}

void vesMultitouchWidget::render(Painter* render)
{

}
