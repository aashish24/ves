/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkAppearance.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkAppearance - Specifies the visual properties of geometry
// .SECTION Description
// vtkAppearance Refer to spec 12.4.1

#ifndef __vtkAppearance_h
#define __vtkAppearance_h
// --------------------------------------------------------------------includes
#include "vtkAppearanceNode.h"
#include "vesSetGet.h"
#include <vector>
// -----------------------------------------------------------------pre-defines
class vtkAppearanceInternal;
// class vtkFillProperties;
// class vtkLineProperties;
// class vtkMaterialNode;
class vtkShaderNode;
// class vtkTextureNode;
// class vtkTextureTransformNode;

// -----------------------------------------------------------------------class
class vtkAppearance : public vtkAppearanceNode
{
public:
  // ............................................................public-methods
  vtkAppearance();
  ~vtkAppearance();
  bool Read();
  void Render(vtkPainter* render);
  vesSetGetMacro(Shader,vtkShaderNode*)

  // vesSetGetMacro(FillProperties, vtkFillProperties*)
  // vesSetGetMacro(LineProperties, vtkLineProperties*)
  // vesSetGetMacro(Material, vtkMaterialNode*)
  // vesSetGetVectorMacro(Shaders,vtkShaderNode*)
  // vesSetGetMacro(Texture, vtkTextureNode*)
  // vesSetGetMacro(TextureTransform, vtkTextureTransformNode*)
protected:
  // ...........................................................protected-ivars
  vtkShaderNode* Shader;

  // vtkFillProperties* FillProperties;
  // vtkLineProperties* LineProperties;
  // vtkMaterialNode* Material;
  // std::vector<vtkShaderNode*> Shaders;
  // vtkTextureNode * Texture;
  // vtkTextureTransformNode* TextureTransform;
protected:
//BTX
  // .......................................................................BTX

private:
  vtkAppearanceInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __vtkAppearance_h
