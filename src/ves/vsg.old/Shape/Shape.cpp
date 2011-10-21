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

#include "vsg/Shape/Shape.h"

#include "vsg/vsgVisitor.h"
#include "Painter.h"
#include "vesMapper.h"

namespace vsg {

  // IMPORTANT: Make sure that this struct has no pointers.  All pointers should
  // be put in the class declaration. For all newly defined pointers make sure
  // to update constructor and destructor methods.
  struct ShapeInternal
  {
    double value; // sample
  };


  Shape::Shape()
  {
    _internal = new ShapeInternal();
  }


  Shape::~Shape()
  {
    delete _internal;
  }


  bool Shape::accept(vsgVisitor* vsgVisitor)
  {
    return vsgVisitor->visitShape(this);
  }


  bool Shape::read()
  {
    //std::cout << "Read: Shape" <<std::endl;
    get_appearance()->read();
    if (get_geometry())
    {
      get_geometry()->read();
    }
    return true;
  }


  void Shape::render(Painter* render)
  {
    render->visitShape(this);
  }


  void Shape::computeBounds()
  {
    vesMapper* mapper = (vesMapper*) get_geometry();
    if(mapper)
    {
      mapper->computeBounds();
      set_BBoxCenter(mapper->get_min(),mapper->get_max());
      set_BBoxSize(mapper->get_min(),mapper->get_max());
    }
  }
}
