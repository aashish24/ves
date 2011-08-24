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

void vesActorCollection::SetTranslation(vesVector3f translation)
{
  set_translation(translation);
}

void vesActorCollection::SetRotation(vesVector4f rotation)
{
  set_rotation(rotation);
}

vesVector3f vesActorCollection::GetTranslation()
{
  return get_translation();
}

vesVector4f vesActorCollection::GetRotation()
{
  return get_rotation();
}
