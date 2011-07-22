/*=========================================================================

 Program:   Visualization Toolkit
 Module:    vesRenderer.mm

 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/

#include "vesRenderer.h"
#include "vesFileReader.h"
#include "vesMultitouchCamera.h"
#include "vesShaderProgram.h"
#include "vesActorCollection.h"
#include "vesCamera.h"
#include "vesGMTL.h"
#include <iostream>
#include <string>

//vesRenderer::vesRenderer(vesShaderProgram* program, vktCamera* camera, vesActor *actor)
//{
//  mProgram = program;
//  mCamera = camera;
//  mActor = actor;
//  once = true;
//}

// -----------------------------------------------------------------------cnstr
vesRenderer::vesRenderer(vesMultitouchCamera* camera)
{
  this->mCamera = camera;
  this->Actor = new vesActorCollection();
  this->_Painter = new Painter();
  this->ActiveCamera = new vesCamera();
  this->ActiveCamera->AddActorCollection(this->Actor);
  vesVector3f center(-100,-200,200);
  vesVector3f translation(-100,-200,-300);
  this->ActiveCamera->SetCenter(center);
  this->ActiveCamera->SetTranslation(translation);
}

// -----------------------------------------------------------------------destr
vesRenderer::~vesRenderer()
{
  delete Actor;
  delete _Painter;
  delete ActiveCamera;
}

// ----------------------------------------------------------------------public
void vesRenderer::AddActor(vesActor* actor)
{
  this->Actor->AddItem(actor);
  once = true;
}

// ----------------------------------------------------------------------public
void vesRenderer::RemoveActor(vesActor* actor)
{
  this->Actor->RemoveItem(actor);
  once = true;
}

// ----------------------------------------------------------------------public
void vesRenderer::Read()
{
  CopyCamera2Model();
  _view = makeTranslationMatrix4x4(vesVector3f(0,0,2))* makeScaleMatrix4x4(.1,.1,.1);
  // vesVector3f position(-100,-200,-300);
  // vesVector3f focalPoint(-100,-200,200);
  // vesVector3f viewUp(0,1,0);
  // _view = vesLookAt(position,focalPoint,viewUp);
  this->_Painter->SetView(_view);
  resize(_width,_height,1);
}

// ----------------------------------------------------------------------public
void vesRenderer::resize(int width, int height, float scale)
{
  if(width > 0 && height >0){
    _width = width;
    _height = height;
  }
  const GLfloat nearp = .1, farp = 10, fov = deg2Rad(45);
  float aspect,left,right,bottom,top;
  if(_width > _height) {
    aspect = _width/_height;
    top = tan(fov) * nearp;
    bottom = -top;
    left = aspect * bottom;
    right = aspect * top;
  }else{
    aspect = _height/_width;
    right = tan(fov) * nearp;
    left = -right;
    bottom = aspect * left;
    top = aspect * right;
  }

  this->Aspect[0] = _width/_height;
  this->Aspect[1] = _height/_width;

  _proj= vesOrtho(left, right, bottom, top, nearp, farp);
  //_proj= vesFrustum(left, right, bottom, top, nearp, farp);
  //_proj = vesPerspective(60,aspect,0,1000);
  this->_Painter->SetProjection(_proj);

  glViewport(0, 0, width, height);

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);

  this->ActiveCamera->Read();
  this->ActiveCamera->ComputeBounds();
  this->ActiveCamera->Normalize();
}

// ----------------------------------------------------------------------public
void vesRenderer::resetView()
{
#if VTK
  this->ResetCamera();
#endif
  _model = vesMatrix4x4f();
  this->_Painter->SetModel(_model);
  mCamera->Reset();
}

// ----------------------------------------------------------------------public
void vesRenderer::CopyCamera2Model()
{
#if GMTL_CAMERA
  _model = mCamera->GetMatrixGMTL();
#else
  _model = mCamera->GetMatrix();
#endif
  this->_Painter->SetModel(_model);
}

// ----------------------------------------------------------------------public
void vesRenderer::Render()
{
  this->Read();
  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  this->ActiveCamera->Render(this->_Painter);
}

// ----------------------------------------------------------------------public
vesCamera* vesRenderer::GetActiveCamera()
{
  return this->ActiveCamera;
}

#if VTK
// ----------------------------------------------------------------------public
void vesRenderer::ResetCamera()
{
  this->Actor->Read();
  this->Actor->ComputeBounds();
  vesVector3f center = this->Actor->GetBBoxCenter();

  double distance;
  vesVector3f vn, vup;
  this->GetActiveCamera();
  if ( this->ActiveCamera != NULL )
  {
    vn = this->ActiveCamera->GetViewPlaneNormal();
  }
  else
  {
    std::cout<< "Trying to reset non-existant camera" << std::endl;
    return;
  }
  double radius = this->Actor->GetBBoxRadius();
  radius = (radius==0)?(.5):(radius);

  double angle=deg2Rad(this->ActiveCamera->GetViewAngle());
  double parallelScale=radius;
  //this->ComputeAspect();
  double aspect[2];
  aspect[0] = this->Aspect[0];
  aspect[1] = this->Aspect[1];

  //this->GetAspect(aspect);
  if(aspect[0]>=1.0) // horizontal window, deal with vertical angle|scale
  {
    if(this->ActiveCamera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)/aspect[0]);
    }
  }
  else // vertical window, deal with horizontal angle|scale
  {
    if(!this->ActiveCamera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)*aspect[0]);
    }
    parallelScale=parallelScale/aspect[0];
  }
  distance =radius/sin(angle*0.5);
  // check view-up vector against view plane normal
  vup = this->ActiveCamera->GetViewUp();
  if ( fabs(gmtl::dot(vup,vn)) > 0.999 )
  {
   // vtkWarningMacro(<<"Resetting view-up since view plane normal is parallel");
    vesVector3f temp;
    temp[0] = -vup[2];
    temp[1] = vup[0];
    temp[2] = vup[1];
    this->ActiveCamera->SetViewUp(temp);
  }
  // update the camera
  this->ActiveCamera->SetFocalPoint(center);
  vesVector3f temp = vn;
  temp*= distance;
  temp+= center;
  this->ActiveCamera->SetPosition(temp);

  float bounds[6];
  bounds[0] = this->Actor->GetMin()[0];
  bounds[1] = this->Actor->GetMax()[0];
  bounds[2] = this->Actor->GetMin()[1];
  bounds[3] = this->Actor->GetMax()[1];
  bounds[4] = this->Actor->GetMin()[2];
  bounds[5] = this->Actor->GetMax()[2];

  this->ResetCameraClippingRange(bounds);
  // setup default parallel scale
  this->ActiveCamera->SetParallelScale(parallelScale);
}

// ----------------------------------------------------------------------public
void vesRenderer::ResetCameraClippingRange(float bounds[6])
{
  vesVector3f  vn, position;
  float  a, b, c, d;
  double  range[2], dist;
  int     i, j, k;

  // Find the plane equation for the camera view plane
  vn = this->ActiveCamera->GetViewPlaneNormal();
  position = this->ActiveCamera->GetPosition();

  a = -vn[0];
  b = -vn[1];
  c = -vn[2];
  d = -(a*position[0] + b*position[1] + c*position[2]);

  // Set the max near clipping plane and the min far clipping plane
  range[0] = a*bounds[0] + b*bounds[2] + c*bounds[4] + d;
  range[1] = 1e-18;

  // Find the closest / farthest bounding box vertex
  for ( k = 0; k < 2; k++ )
  {
    for ( j = 0; j < 2; j++ )
    {
      for ( i = 0; i < 2; i++ )
      {
        dist = a*bounds[i] + b*bounds[2+j] + c*bounds[4+k] + d;
        range[0] = (dist<range[0])?(dist):(range[0]);
        range[1] = (dist>range[1])?(dist):(range[1]);
      }
    }
  }

  // Do not let the range behind the camera throw off the calculation.
  if (range[0] < 0.0)
  {
    range[0] = 0.0;
  }
  // Give ourselves a little breathing room
  range[0] = 0.99*range[0] - (range[1] - range[0])*0.5;
  range[1] = 1.01*range[1] + (range[1] - range[0])*0.5;

  // Make sure near is not bigger than far
  range[0] = (range[0] >= range[1])?(0.01*range[1]):(range[0]);

  float NearClippingPlaneTolerance = 0.01;

  // make sure the front clipping range is not too far from the far clippnig
  // range, this is to make sure that the zbuffer resolution is effectively
  // used

  if (range[0] < NearClippingPlaneTolerance*range[1])
  {
    range[0] = NearClippingPlaneTolerance*range[1];
  }
  this->ActiveCamera->SetClippingRange( range[0],range[1] );
}
#endif
