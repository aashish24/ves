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
class vesTriangleData;

class vtkPolyDataToTriangleData
{
public:
  static vesTriangleData* Convert(vtkPolyData* input);
  static void Convert(vtkPolyData* input, vesTriangleData* output);

  // This function is designed to be as fast as possible at the expense of
  // generality.  It only converts triangle data.  It takes about 0.2 of the time
  // as the general ConvertFunction.
  //
  // Note: many of the optimizations here could be added to the Convert function
  //       this would be wortwhile future work.
  static void ConvertTriangles(vtkPolyData* input, vesTriangleData* output);

  // This is a convenience method for populating the vertex colors array on a
  // vesTriangleData object.  First, it looks on the given polydata for an
  // unsigned char array named rgb_colors.  If that is not found, it will
  // looks for a point data array with a single component.  If such an array is found,
  // a blue to red vtkLookupTable is used to generate vertex color values from
  // the scalar array. This will use the first suitable array that is found.
  // If no array is found, this method doesn't do anything.
  static void ComputeVertexColorFromScalars(vtkPolyData* polyData, vesTriangleData* triangleData);

};
