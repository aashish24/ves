/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Appearance.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME Appearance - Specifies the visual properties of geometry
// .SECTION Description
// Appearance Refer to spec 12.4.1

#ifndef __Appearance_h
#define __Appearance_h
// --------------------------------------------------------------------includes
#include "vsgAppearanceNode.h"
#include "vesSetGet.h"
#include <vector>
// -----------------------------------------------------------------pre-defines
class AppearanceInternal;
// class vtkFillProperties;
// class vtkLineProperties;
// class vtkMaterialNode;
class vsgShaderNode;
// class vtkTextureNode;
// class vtkTextureTransformNode;

// -----------------------------------------------------------------------class
class Appearance : public vsgAppearanceNode
{
public:
  // ............................................................public-methods
  Appearance();
  ~Appearance();
  bool Read();
  void Render(Painter* render);
  vesSetGetMacro(Shader,vsgShaderNode*)

  // vesSetGetMacro(FillProperties, vtkFillProperties*)
  // vesSetGetMacro(LineProperties, vtkLineProperties*)
  // vesSetGetMacro(Material, vtkMaterialNode*)
  // vesSetGetVectorMacro(Shaders,vsgShaderNode*)
  // vesSetGetMacro(Texture, vtkTextureNode*)
  // vesSetGetMacro(TextureTransform, vtkTextureTransformNode*)
protected:
  // ...........................................................protected-ivars
  vsgShaderNode* Shader;

  // vtkFillProperties* FillProperties;
  // vtkLineProperties* LineProperties;
  // vtkMaterialNode* Material;
  // std::vector<vsgShaderNode*> Shaders;
  // vtkTextureNode * Texture;
  // vtkTextureTransformNode* TextureTransform;
protected:
//BTX
  // .......................................................................BTX

private:
  AppearanceInternal *Internal;
//ETX
  // .......................................................................ETX
};

#endif // __Appearance_h
