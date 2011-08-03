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
  MFNode actorList;
  actorList.push_back(a);
  addChildren(actorList);
}

// ----------------------------------------------------------------------public
void vesActorCollection::RemoveItem(vesActor* a)
{
  MFNode actorList;
  actorList.push_back(a);
  removeChildren(actorList);
}

bool vesActorCollection::Read()
{
  //std::cout << "Read: Actor Collection" <<std::endl;

  for (int i =0; i<this->get_children().size(); ++i)
  {
    vesActor* child = (vesActor*) this->get_children()[i];
    child->Read();
  }
  return true;
}

 vesMatrix4x4f vesActorCollection::Eval()
 {
   return Transform::eval();
 }

void vesActorCollection::Render(Painter *render)
{
  render->ActorCollection(this);
}

void vesActorCollection::ComputeBounds()
{
  vesVector3f allMin(0,0,0);
  vesVector3f allMax(0,0,0);

  for (int i =0; i<this->get_children().size(); ++i)
    {
    vesActor* child = (vesActor*) this->get_children()[i];
    child->ComputeBounds();
    vesVector3f min = child->get_min();
    vesVector3f max = child->get_max();

    if (i == 0)
      {
      allMin = min;
      allMax = max;
      }

    for (int i = 0; i < 3; ++i)
      {
      if (max[i] > allMax[i])
        {
        allMax[i] = max[i];
        }
      if (min[i] < allMin[i])
        {
        allMin[i] = min[i];
        }
      }
    }

  set_BBoxCenter(allMin, allMax);
  set_BBoxSize(allMin, allMax);
}

void vesActor::SetTranslation(vesVector3f translation)
{
  set_translation(translation);
}

void vesActor::SetRotation(vesVector4f rotation)
{
  set_rotation(rotation);
}

vesVector3f vesActor::GetTranslation()
{
  return get_translation();
}

vesVector4f vesActor::GetRotation()
{
  return get_rotation();
}
