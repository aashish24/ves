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
#include <iostream>
#include "vtkPainter.h"

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
  std::vector<vtkChildNode*> actorList;
  actorList.push_back(a);
  AddChildren(actorList);
}

// ----------------------------------------------------------------------public
void vtkActorCollection::RemoveItem(vtkActor* a)
{
  std::vector<vtkChildNode*> actorList;
  actorList.push_back(a);
  RemoveChildren(actorList);
}
// void vtkActorCollection::Handle(vtkController *handle)
// {
// }

bool vtkActorCollection::Read()
{
  std::cout << "Read: Actor Collection" <<std::endl;

  for (int i =0; i<this->Children.size(); ++i)
  {
    vtkActor* child = (vtkActor*) this->Children[i];
    child->Read();
  }
  return true;
}

 vtkMatrix4x4f vtkActorCollection::Eval()
 {
   return vtkTransform::Eval()*NormalizedMatrix;
 }

void vtkActorCollection::Render(vtkPainter *render)
{
  render->ActorCollection(this);
}

void vtkActorCollection::ComputeBounds()
{
  for (int i =0; i<this->Children.size(); ++i)
  {
    vtkActor* child = (vtkActor*) this->Children[i];
    child->ComputeBounds();
    vtkVector3f min = child->GetMin();
    vtkVector3f max = child->GetMax();
    for (int i = 0; i < 3; ++i)
      {
        (max[i] > GetMax()[i]) ? max[i] = max[i]: max[i] = GetMax()[i];
        (min[i] < GetMin()[i]) ? min[i] = min[i]: min[i] = GetMin()[i];
      }
    SetBBoxCenter(min,max);
    SetBBoxSize(min,max);
  }
}

void vtkActorCollection::Normalize()
{
  float r = GetBBoxRadius();
  this->NormalizedMatrix =
  makeScaleMatrix4x4(1/r,1/r,1/r)*
  makeTranslationMatrix4x4(-GetBBoxCenter());
  SetBBoxCenter(transformPoint3f(this->NormalizedMatrix, GetBBoxCenter()));
  SetBBoxSize(transformPoint3f(this->NormalizedMatrix, GetBBoxSize()));
  std::cout<< "BBoxSize = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxSize()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;
  
  std::cout<< "BBoxCenter = [ ";
  for (int i =0 ; i<3; ++i) {
    std::cout<<GetBBoxCenter()[i]<< " ";
  }
  std::cout<<"]"<<std::endl;
}
