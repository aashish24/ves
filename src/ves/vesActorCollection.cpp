/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesActorCollection.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vesActorCollection.h"

// --------------------------------------------------------------------includes
#include "vesActor.h"
#include <iostream>
#include "Painter.h"

// -----------------------------------------------------------------------macro

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vesActorCollectionInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vesActorCollection::vesActorCollection()
{
  this->Internal = new vesActorCollectionInternal();
}

// -----------------------------------------------------------------------destr
vesActorCollection::~vesActorCollection()
{
  delete this->Internal;
}

// ----------------------------------------------------------------------public
void vesActorCollection::AddItem(vesActor* a)
{
  std::vector<vsgChildNode*> actorList;
  actorList.push_back(a);
  AddChildren(actorList);
}

// ----------------------------------------------------------------------public
void vesActorCollection::RemoveItem(vesActor* a)
{
  std::vector<vsgChildNode*> actorList;
  actorList.push_back(a);
  RemoveChildren(actorList);
}

bool vesActorCollection::Read()
{
  //std::cout << "Read: Actor Collection" <<std::endl;

  for (int i =0; i<this->Children.size(); ++i)
  {
    vesActor* child = (vesActor*) this->Children[i];
    child->Read();
  }
  return true;
}

 vesMatrix4x4f vesActorCollection::Eval()
 {
   return Transform::Eval()*NormalizedMatrix;
 }

void vesActorCollection::Render(Painter *render)
{
  render->ActorCollection(this);
}

void vesActorCollection::ComputeBounds()
{
  for (int i =0; i<this->Children.size(); ++i)
  {
    vesActor* child = (vesActor*) this->Children[i];
    child->ComputeBounds();
    vesVector3f min = child->GetMin();
    vesVector3f max = child->GetMax();
    for (int i = 0; i < 3; ++i)
      {
        (max[i] > GetMax()[i]) ? max[i] = max[i]: max[i] = GetMax()[i];
        (min[i] < GetMin()[i]) ? min[i] = min[i]: min[i] = GetMin()[i];
      }
    SetBBoxCenter(min,max);
    SetBBoxSize(min,max);
  }
}

void vesActorCollection::Normalize()
{
  float r = GetBBoxRadius();
  this->NormalizedMatrix =
  makeScaleMatrix4x4(1/r,1/r,1/r)*
  makeTranslationMatrix4x4(-GetBBoxCenter());
  SetBBoxCenter(transformPoint3f(this->NormalizedMatrix, GetBBoxCenter()));
  SetBBoxSize(transformPoint3f(this->NormalizedMatrix, GetBBoxSize()));
}
