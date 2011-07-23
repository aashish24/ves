

class vtkPolyData;
class vesTriangleData;

class vesPolyDataToTriangleData
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
};
