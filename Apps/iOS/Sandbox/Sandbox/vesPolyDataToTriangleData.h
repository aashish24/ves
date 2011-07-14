

class vtkPolyData;
class vesTriangleData;

class vesPolyDataToTriangleData
{
public:
    static vesTriangleData* Convert(vtkPolyData* input);
};
