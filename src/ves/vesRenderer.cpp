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
#include "gmtl/Generate.h"
#include "gmtl/AxisAngle.h"
#include <iostream>
#include <string>

namespace {
void PrintMatrix(std::string name, vesMatrix4x4f mv)
{
  std::cerr << name << ":" << std::endl;
  for (int i = 0; i < 4; ++i)
    {
    std::cerr << mv[i][0] << "," << mv[i][1] << "," << mv[i][2] << "," << mv[i][3] << std::endl;
    }
  std::cerr << std::endl;
}
}

// -----------------------------------------------------------------------cnstr
vesRenderer::vesRenderer()
{
  this->Actor = new vesActorCollection();
  this->Paint = new Painter();
  this->Camera = new vesCamera();
  this->Aspect[0] = this->Aspect[1] = 1.0;
  this->Width = 100.0f;
  this->Height = 100.0f;
}

// -----------------------------------------------------------------------destr
vesRenderer::~vesRenderer()
{
  delete Actor;
  delete Paint;
  delete Camera;
}

// ----------------------------------------------------------------------public
void vesRenderer::AddActor(vesActor* actor)
{
  this->Actor->AddItem(actor);
}

// ----------------------------------------------------------------------public
void vesRenderer::RemoveActor(vesActor* actor)
{
  this->Actor->RemoveItem(actor);
}

// ----------------------------------------------------------------------public
void vesRenderer::Render()
{
  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);
  vesMatrix4x4f proj = this->Camera->ComputeProjectionTransform(this->Aspect[1], -1, 1);
  vesMatrix4x4f view = this->Camera->ComputeViewTransform();
  this->Paint->Push(proj);
  this->Paint->Push(view);
  this->Actor->Render(this->Paint);
  this->Paint->Pop();
  this->Paint->Pop();
}

// ----------------------------------------------------------------------public
void vesRenderer::Resize(int width, int height, float scale)
{
  this->Width = (width > 0) ? width : 1;
  this->Height = (height > 0) ? height : 1;
  this->Aspect[0] = static_cast<double>(this->Height)/this->Width;
  this->Aspect[1] = static_cast<double>(this->Width)/this->Height;
}

// ----------------------------------------------------------------------public
vesVector3f vesRenderer::ComputeWorldToDisplay(vesVector3f world)
{
  //
  // WorldToView
  //
  vesMatrix4x4f proj_mat = this->Camera->ComputeProjectionTransform(this->Aspect[1], 0, 1);
  vesMatrix4x4f view_mat = this->Camera->ComputeViewTransform();
  vesMatrix4x4f mat = proj_mat*view_mat;
  vesVector4f world4(world[0], world[1], world[2], 1);
  vesVector4f view;
  gmtl::xform(view, mat, world4);
  view[0] /= view[3];
  view[1] /= view[3];
  view[2] /= view[3];

  //
  // ViewToDisplay
  //
  vesVector3f display;
  display[0] = (view[0] + 1.0f) * this->Width / 2.0f;
  display[1] = (view[1] + 1.0f) * this->Height / 2.0f;
  display[2] = view[2];
  return display;
}

// ----------------------------------------------------------------------public
vesVector3f vesRenderer::ComputeDisplayToWorld(vesVector3f display)
{
  //
  // DisplayToView
  //
  vesVector4f view;
  view[0] = 2.0f * display[0] / this->Width - 1.0f;
  view[1] = 2.0f * display[1] / this->Height - 1.0f;
  view[2] = display[2];
  view[3] = 1;

  //
  // ViewToWorld
  //
  vesMatrix4x4f proj_mat = this->Camera->ComputeProjectionTransform(this->Aspect[1], 0, 1);
  vesMatrix4x4f view_mat = this->Camera->ComputeViewTransform();
  vesMatrix4x4f mat = proj_mat*view_mat;
  vesMatrix4x4f inv;
  gmtl::invertFull(inv, mat);
  vesVector4f world4;
  gmtl::xform(world4, inv, view);

  vesVector3f world(world4[0]/world4[3], world4[1]/world4[3], world4[2]/world4[3]);
  return world;
}

// ----------------------------------------------------------------------public
void vesRenderer::ResetCamera()
{
  this->Actor->Read();
  this->Actor->ComputeBounds();
  vesVector3f center = this->Actor->get_BBoxCenter();

  double distance;
  vesVector3f vn, vup;
  if ( this->Camera != NULL )
  {
    vn = this->Camera->GetViewPlaneNormal();
  }
  else
  {
    std::cout<< "Trying to reset non-existant camera" << std::endl;
    return;
  }
  double radius = this->Actor->GetBBoxRadius();
  radius = (radius==0)?(.5):(radius);

  double angle=deg2Rad(this->Camera->GetViewAngle());
  double parallelScale=radius;
  //this->ComputeAspect();
  if(Aspect[0]>=1.0) // horizontal window, deal with vertical angle|scale
  {
    if(this->Camera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)/Aspect[0]);
    }
  }
  else // vertical window, deal with horizontal angle|scale
  {
    if(!this->Camera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)*Aspect[0]);
    }
    parallelScale=parallelScale/Aspect[0];
  }
  distance =radius/sin(angle*0.5);
  // check view-up vector against view plane normal
  vup = this->Camera->GetViewUp();
  if ( fabs(gmtl::dot(vup,vn)) > 0.999 )
  {
   // vtkWarningMacro(<<"Resetting view-up since view plane normal is parallel");
    vesVector3f temp;
    temp[0] = -vup[2];
    temp[1] = vup[0];
    temp[2] = vup[1];
    this->Camera->SetViewUp(temp);
  }
  // update the camera
  this->Camera->SetFocalPoint(center);
  vesVector3f temp = vn;
  temp*= distance;
  temp+= center;
  this->Camera->SetPosition(temp);

  float bounds[6];
  bounds[0] = this->Actor->get_min()[0];
  bounds[1] = this->Actor->get_max()[0];
  bounds[2] = this->Actor->get_min()[1];
  bounds[3] = this->Actor->get_max()[1];
  bounds[4] = this->Actor->get_min()[2];
  bounds[5] = this->Actor->get_max()[2];

  this->ResetCameraClippingRange(bounds);
  // setup default parallel scale
  this->Camera->SetParallelScale(parallelScale);
}

// ----------------------------------------------------------------------public
void vesRenderer::ResetCameraClippingRange()
{
  this->Actor->Read();
  this->Actor->ComputeBounds();
  float bounds[6];
  bounds[0] = this->Actor->get_min()[0];
  bounds[1] = this->Actor->get_max()[0];
  bounds[2] = this->Actor->get_min()[1];
  bounds[3] = this->Actor->get_max()[1];
  bounds[4] = this->Actor->get_min()[2];
  bounds[5] = this->Actor->get_max()[2];

  this->ResetCameraClippingRange(bounds);
}

// ----------------------------------------------------------------------
void vesRenderer::ResetCameraClippingRange(float bounds[6])
{
  vesVector3f  vn, position;
  float  a, b, c, d;
  double  range[2], dist;
  int     i, j, k;

  // Find the plane equation for the camera view plane
  vn = this->Camera->GetViewPlaneNormal();
  position = this->Camera->GetPosition();

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
  this->Camera->SetClippingRange( range[0],range[1] );
}
