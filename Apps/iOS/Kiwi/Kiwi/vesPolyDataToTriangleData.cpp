
#include "vesPolyDataToTriangleData.h"

#include "vtkCellArray.h"
#include "vesTriangleData.h"
#include "vesGMTL.h"
#include "vtkPointData.h"
#include "vtkPolyData.h"

vesTriangleData* vesPolyDataToTriangleData::Convert(vtkPolyData* input)
{
    //cerr << "starting conversion" << endl;
    vesTriangleData* output = new vesTriangleData();
    output->GetPoints().resize(input->GetNumberOfPoints());    
    vtkVertex3f* v = &output->GetPoints()[0];
	for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v){
		v->point[0] = input->GetPoint(i)[0];
		v->point[1] = input->GetPoint(i)[1];
		v->point[2] = input->GetPoint(i)[2];
	}

    vtkCellArray* polys = input->GetPolys();
    vtkIdType num;
    vtkIdType* vertices;
    polys->InitTraversal();
    for (int i = 0; i < polys->GetNumberOfCells(); ++i) {
        polys->GetNextCell(num, vertices);
        if (num == 3) {
            vesVector3us indices;
            indices[0] = vertices[0];
            indices[1] = vertices[1];
            indices[2] = vertices[2];
            output->GetTriangles().push_back(indices);
        }
        else if (num == 4) {
            vesVector3us indices1;
            vesVector3us indices2;
            indices1[0] = vertices[0];
            indices1[1] = vertices[1];
            indices1[2] = vertices[2];
            indices2[0] = vertices[3];
            indices2[1] = vertices[0];
            indices2[2] = vertices[2];
            output->GetTriangles().push_back(indices1);
            output->GetTriangles().push_back(indices2);
        }
    }
    
    vtkCellArray* strips = input->GetStrips();
    strips->InitTraversal();
    for (int i = 0; i < strips->GetNumberOfCells(); ++i) {
        strips->GetNextCell(num, vertices);
        for (int i = 2; i < num; ++i)
          {
          vesVector3us indices;
          indices[0] = vertices[i-2];
          indices[1] = vertices[i-1];
          indices[2] = vertices[i];
          output->GetTriangles().push_back(indices);
          }
    }
    
    if (input->GetPointData()->GetNormals()) {
        vtkDataArray* normals = input->GetPointData()->GetNormals();
        v = &output->GetPoints()[0];
        for (int i = 0; i < input->GetNumberOfPoints(); ++i, ++v) {
            v->normal[0] = normals->GetTuple(i)[0];
            v->normal[1] = normals->GetTuple(i)[1];
            v->normal[2] = normals->GetTuple(i)[2];
        }
    } else {
        output->ComputeNormals();
    }
    //cerr << "done with conversion" << endl;
    return output;
}
