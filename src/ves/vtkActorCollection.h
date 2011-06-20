/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkActorCollection.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkActorCollection - Transformation object holding a collection of Actors
// .SECTION Description
// vtkActorCollection

#ifndef __vtkActorCollection_h
#define __vtkActorCollection_h
// --------------------------------------------------------------------includes
#include "vtkTransform.h"

// -----------------------------------------------------------------pre-defines
class vtkActorCollectionInternal;
class vtkActor;

// -----------------------------------------------------------------------class
class vtkActorCollection : public vtkTransform
{
public:
  // ............................................................public-methods
  vtkActorCollection();
  ~vtkActorCollection();
  void AddItem(vtkActor* a);
  //void Handle(vtkController *handle);
  bool Read();
  //vtkMatrix4x4f Eval();
  void Render(vtkPainter *render);
protected:
  // ...........................................................protected-ivars

protected:
//BTX
  // .......................................................................BTX
private:
  vtkActorCollectionInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vtkActorCollection_h
