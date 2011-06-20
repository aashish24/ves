/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkRenderer.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkRenderer.h"

// --------------------------------------------------------------------includes
#include "vtkObjectFactory.h"

// -----------------------------------------------------------------------macro
// Defines standard new macro
vtkStandardNewMacro(vtkRenderer);

// -----------------------------------------------------------------------macro
// Needed when we don't use vtkStandardNewMacro
// vtkInstantiatorNewMacro(vtkRenderer);

// --------------------------------------------------------------------internal
// IMPORTANT: Make sure that this struct has no pointers.  All pointers should
// be put in the class declaration. For all newly defined pointers make sure to
// update constructor and destructor methods.
struct vtkRendererInternal
{
  double value; // sample
};

// -----------------------------------------------------------------------cnstr
vtkRenderer::vtkRenderer()
{
  this->Internal = new vtkRendererInternal();
}

// -----------------------------------------------------------------------destr
vtkRenderer::~vtkRenderer()
{
  delete this->Interna;l
}

// -----------------------------------------------------------------------print
void vtkRenderer::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Double: (" << this->vtkRendererInternal->value << ")"
     << endl;
}

