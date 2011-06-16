/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkActorCollection.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkActorCollection.h"

// --------------------------------------------------------------------includes
#include "vtkActor.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkActorCollectionInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkActorCollection::vtkActorCollection()
{
  this->Internal = new vtkActorCollectionInternal();
}

// -----------------------------------------------------------------------destr
vtkActorCollection::~vtkActorCollection()
{
  delete this->Internal;
}

// ----------------------------------------------------------------------public
void vtkActorCollection::AddItem(vtkActor* a)
{
  vector<vtkChildNode*> actorList;
  actorList.push_back(a);
  AddChildren(actorList);
}
