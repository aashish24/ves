/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkRenderer.mm
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#include "vtkRenderer.h"
#include "vtkFileReader.h"
#include "vtkMultitouchCamera.h"
#include "vtkShaderProgram.h"
#include "vtkActorCollection.h"

#include <iostream>
#include <string>

//vtkRenderer::vtkRenderer(vtkShaderProgram* program, vktCamera* camera, vtkActor *actor)
//{
//  mProgram = program;
//  mCamera = camera;
//  mActor = actor;
//  once = true;
//}

vtkRenderer::vtkRenderer(vtkMultitouchCamera* camera)
{
  this->mCamera = camera;
  this->Actor = new vtkActorCollection();
  this->Painter = new vtkPainter();
}
vtkRenderer::~vtkRenderer()
{
  delete Actor;
  delete Painter;
}

void vtkRenderer::AddActor(vtkActor* actor)
{
  this->Actor->AddItem(actor);
  once = true;
}

void vtkRenderer::Read()
{
  CopyCamera2Model();
  this->Actor->Read();
  _view = makeTranslationMatrix4x4(vtkVector3f(0,0,2))* makeScaleMatrix4x4(.1,.1,.1);
  this->Painter->SetView(_view);
  resize(_width,_height,1);
}

void vtkRenderer::resize(int width, int height, float scale)
{
  if(width > 0 && height >0){
    _width = width;
    _height = height;
  }
  const GLfloat nearp = .1, farp = 10, fov = 90*M_PI/360.0*scale*.5;
  float aspect,left,right,bottom,top;
  if(_width > _height) {
    aspect = _width/_height;
    top = tan(fov) * nearp;
    bottom = -top;
    left = aspect * bottom;
    right = aspect * top;
  }else{
    aspect = _height/width;
    right = tan(fov) * nearp;
    left = -right;
    bottom = aspect * left;
    top = aspect * right;
  }
  _proj= makeOrthoMatrix4x4(left, right, bottom, top, nearp, farp);
  //_proj= makePerspectiveMatrix4x4(left, right, bottom, top, nearp, farp);
  
  this->Painter->SetProjection(_proj);
  glViewport(0, 0, width, height);

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
}

void vtkRenderer::resetView()
{
  _model = vtkMatrix4x4f();
  this->Painter->SetModel(_model);
  mCamera->Reset();	
}

void vtkRenderer::CopyCamera2Model()
{
#if GMTL_CAMERA
  _model = mCamera->GetMatrixGMTL();
#else
  _model = mCamera->GetMatrix();
#endif  
  this->Painter->SetModel(_model);
}
void vtkRenderer::Render()
{
  this->Read();
  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  this->Actor->Render(this->Painter);
#if MOVE_THIS 
  // Work out the appropriate matrices
  vtkMatrix4x4f mvp;
  //mvp = _proj * _view * _model * (*mActor)();
  vtkMatrix4x4f mv;
  mv = _view * _model * Actor->Eval();
  mvp = _proj * mv;
	
  vtkMatrix3x3f normal_matrix = makeNormalMatrix3x3f(makeTransposeMatrix4x4(makeInverseMatrix4x4 (mv)));
  //vtkMatrix4x4f temp = makeNormalizedMatrix4x4(makeTransposeMatrix4x4(_vie);
  vtkPoint3f lightDir = vtkPoint3f(0.0,0.0,.650);
	
  vtkVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);
  this->mProgram->SetUniformMatrix4x4f("u_mvpMatrix",mvp);
  this->mProgram->SetUniformMatrix3x3f("u_normalMatrix",normal_matrix);
  this->mProgram->SetUniformVector3f("u_ecLightDir",light);

  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Enable our attribute arrays
  this->mProgram->EnableVertexArray("a_vertex");
  this->mProgram->EnableVertexArray("a_normal");

  if (mActor) {
    mActor->Render(this->mProgram);
  }

  //glBindBuffer(GL_ARRAY_BUFFER, 0);
  //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  this->mProgram->DisableVertexArray("a_vertex");
  this->mProgram->DisableVertexArray("a_normal");
#endif
}
