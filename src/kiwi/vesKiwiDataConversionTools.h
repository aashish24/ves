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
/// \class vesKiwiDataConversionTools
/// \ingroup KiwiPlatform
#ifndef __vesKiwiDataConversionTools_h
#define __vesKiwiDataConversionTools_h

class vtkPolyData;
class vtkDataSet;
class vtkDiscretizableColorTransferFunction;
class vtkLookupTable;
class vtkUnsignedCharArray;
class vtkDataArray;
class vtkScalarsToColors;

class vesGeometryData;
class vesTexture;

#include <vesSharedPtr.h>

#include <vtkSmartPointer.h>

class vesKiwiDataConversionTools
{
public:
  static vesSharedPtr<vesGeometryData> Convert(vtkPolyData* input);
  template<typename T>
  static vesSharedPtr<vesGeometryData> GenericConvert(vtkPolyData* input);
  static void Convert(vtkPolyData* input, vesSharedPtr<vesGeometryData> output);

  // This function is designed to be as fast as possible at the expense of
  // generality.  It only converts triangle data.  It takes about 0.2 of the time
  // as the general ConvertFunction.
  //
  // Note: many of the optimizations here could be added to the Convert function
  //       this would be wortwhile future work.
  static void ConvertTriangles(vtkPolyData* input,
    vesSharedPtr<vesGeometryData> output);
  template<typename T>
  static void GenericConvertTriangles(vtkPolyData* input,
    vesSharedPtr<vesGeometryData> output);

  /// Convert point and scalar data to VES format, it sacrifices generality for
  /// speed and conversion of specific types useful for point clouds.
  static vesSharedPtr<vesGeometryData> ConvertPoints(vtkPolyData* input);

  static vtkUnsignedCharArray* FindRGBColorsArray(vtkDataSet* dataSet);
  static vtkDataArray* FindScalarsArray(vtkDataSet* dataSet);
  static vtkDataArray* FindTextureCoordinatesArray(vtkDataSet* dataSet);

  static vtkSmartPointer<vtkDiscretizableColorTransferFunction> GetBlackBodyRadiationColorMap(double scalarRange[2]);
  static vtkSmartPointer<vtkLookupTable> GetRedToBlueLookupTable(double scalarRange[2]);
  static vtkSmartPointer<vtkLookupTable> GetGrayscaleLookupTable(double scalarRange[2]);
  static void SetVertexColors(vtkUnsignedCharArray* colors,
    vesSharedPtr<vesGeometryData> triangleData);
  static void SetVertexColors(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors,
    vesSharedPtr<vesGeometryData> triangleData);
  static void SetTextureCoordinates(
    vtkDataArray* tcoords, vesSharedPtr<vesGeometryData> triangleData);

  static vtkSmartPointer<vtkUnsignedCharArray> MapScalars(vtkDataArray* scalars, vtkScalarsToColors* scalarsToColors);
  static void SetTextureData(vtkUnsignedCharArray* pixels,
    vesSharedPtr<vesTexture> texture, int width, int height);
};

#endif
