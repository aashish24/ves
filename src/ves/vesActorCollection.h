/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vesActorCollection.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vesActorCollection - Transformation object holding a collection of Actors
// .SECTION Description
// vesActorCollection

#ifndef __vesActorCollection_h
#define __vesActorCollection_h
// --------------------------------------------------------------------includes
#include "vsg/Grouping/Transform.h"

// -----------------------------------------------------------------pre-defines
class vesActorCollectionInternal;
class vesActor;

// -----------------------------------------------------------------------class
class vesActorCollection : public vsg::Transform
{
public:
  // ............................................................public-methods
  vesActorCollection();
  ~vesActorCollection();
  void AddItem(vesActor* a);
  void RemoveItem(vesActor* a);
  void SetTranslation(vesVector3f translation);
  void SetRotation(vesVector4f rotation);
  vesVector3f GetTranslation();
  vesVector4f GetRotation();

  //void Handle(vesController *handle);
  bool Read();
  vesMatrix4x4f Eval();
  void Render(Painter *render);
  void ComputeBounds();
protected:
  // ...........................................................protected-ivars
protected:
//BTX
  // .......................................................................BTX
private:
  vesActorCollectionInternal *Internal;
//ETX
  // .......................................................................ETX


};

#endif // __vesActorCollection_h
