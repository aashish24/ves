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

class vtkPolyData;
class vtkDataSet;
class vtkDiscretizableColorTransferFunction;
class vtkLookupTable;
class vtkUnsignedCharArray;
class vtkDataArray;
class vtkScalarsToColors;

class vesTriangleData;
class vesTexture;

#include <vesSharedPtr.h>

#include <vtkSmartPointer.h>

class vesDataConversionTools
{
public:
  static vesSharedPtr<vesTriangleData> Convert(vtkPolyData* input);
  static void Convert(vtkPolyData* input, vesSharedPtr<vesTriangleData> output);

  // This function is designed to be as fast as possible at the expense of
  // generality.  It only converts triangle data.  It takes about 0.2 of the time
  // as the general ConvertFunction.
  //
  // Note: many of the optimizations here could be added to the Convert function
  //       this would be wortwhile future work.
  static void ConvertTriangles(vtkPolyData* input,
    vesSharedPtr<vesTriangleData> output);


  static vtkUnsignedCharArray* FindRGBColorsArray(vtkDataSet* dataSet);
  static vtkDataArray* FindScalarsArray(vtkDataSet* dataSet);
  static vtkDataArray* FindTextureCoordinatesArray(vtkDataSet* dataSet);

  static vtkSmartPointer<vtkDiscretizableColorTransferFunction> GetBlackBodyRadiationColorMap(double scalarRange[2]);
  static vtkSmartPointer<vtkLookupTable> GetRedToBlueLookupTable(double scalarRange[2]);
  static vtkSmartPointer<vtkLookupTable> GetGrayscaleLookupTable(double scalarRange[2]);
  static void SetVertexColors(vtkUnsignedCharArray* colors,
    vesSharedPtr<vesTriangleData> triangleData);
  static void SetVertexColors(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors,
    vesSharedPtr<vesTriangleData> triangleData);
  static void SetTextureCoordinates(
    vtkDataArray* tcoords, vesSharedPtr<vesTriangleData> triangleData);

  static vtkSmartPointer<vtkUnsignedCharArray> MapScalars(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors);
  static void SetTextureData(vtkUnsignedCharArray* pixels,
    vesSharedPtr<vesTexture> texture, int width, int height);

};
