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
  bool read();
  vesMatrix4x4f Eval();
  void render(Painter *render);
  void computeBounds();
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
