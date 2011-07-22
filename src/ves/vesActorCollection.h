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
#include "Transform.h"

// -----------------------------------------------------------------pre-defines
class vesActorCollectionInternal;
class vesActor;

// -----------------------------------------------------------------------class
class vesActorCollection : public Transform
{
public:
  // ............................................................public-methods
  vesActorCollection();
  ~vesActorCollection();
  void AddItem(vesActor* a);
  void RemoveItem(vesActor* a);
  //void Handle(vesController *handle);
  bool Read();
  vesMatrix4x4f Eval();
  void Render(Painter *render);
  void ComputeBounds();
  vesGetMacro(Min, vesVector3f)
  vesGetMacro(Max, vesVector3f)
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
