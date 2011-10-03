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

#include "vesRenderer.h"
#include "vesFileReader.h"
#include "vesMultitouchCamera.h"
#include "vesShaderProgram.h"
#include "vesActorCollection.h"
#include "vesCamera.h"
#include "vesGMTL.h"
#include "vesVisitor.h"

#include "gmtl/Generate.h"
#include "gmtl/AxisAngle.h"

// C++ includes
#include <iostream>
#include <string>

namespace {
void PrintMatrix(std::string name, vesMatrix4x4f mv)
{
  std::cerr << name << ":" << std::endl;
  for (int i = 0; i < 4; ++i)
  {
    std::cerr << mv[i][0] << "," << mv[i][1] << "," << mv[i][2] << ","
              << mv[i][3] << std::endl;
  }
  std::cerr << std::endl;
}
}

vesRenderer::vesRenderer()
{
  this->m_width  = 100;
  this->m_height = 100;

  this->m_aspect[0] = this->m_aspect[1] = 1.0;

  this->m_backgroundColor[0] = 0.5f;
  this->m_backgroundColor[1] = 0.5f;
  this->m_backgroundColor[2] = 0.5f;
  this->m_backgroundColor[3] = 1.0f;

  this->m_camera    = new vesCamera();
  this->m_sceneRoot = 0x0;
}


vesRenderer::~vesRenderer()
{
//  delete Actor;
//  delete Paint;
  delete this->m_camera; this->m_camera = 0x0;
}


void vesRenderer::setSceneRoot(vesActor *root)
{
  // \todo: Release graphics resources for the last scene root.
  this->m_sceneRoot = root;
}


void vesRenderer::render()
{
  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(this->m_backgroundColor[0], this->m_backgroundColor[1],
               this->m_backgroundColor[2], this->m_backgroundColor[3]);

  glEnable(GL_DEPTH_TEST);

  vesMatrix4x4f projectionMatrix =
    this->m_camera->ComputeProjectionTransform(this->m_aspect[1],
                                                                -1, 1);
  vesMatrix4x4f viewMatrix =
    this->m_camera->ComputeViewTransform();

  // Cull traversal.
  vesVisitor cullVisitor(vesVisitor::CullVisitor,
                         vesVisitor::TraverseAllChildren);
  if (this->m_sceneRoot) {
    cullVisitor.pushModelViewMatrix(viewMatrix);
    cullVisitor.pushProjectionMatrix(projectionMatrix);
    cullVisitor.visit(*this->m_sceneRoot);
  }

//  this->Paint->push(proj);
//  this->Paint->push(view);
//  this->Actor->render(this->Paint);
//  this->Paint->pop();
//  this->Paint->pop();
}


void vesRenderer::resize(int width, int height, float scale)
{
  this->m_width  = (width  > 0) ? width  : 1;
  this->m_height = (height > 0) ? height : 1;

  this->m_aspect[0] = static_cast<double>(this->m_height)/this->m_width;
  this->m_aspect[1] = static_cast<double>(this->m_width) /this->m_height;
}


vesVector3f vesRenderer::computeWorldToDisplay(vesVector3f world)
{
  // WorldToView
  vesMatrix4x4f proj_mat = this->m_camera->ComputeProjectionTransform(this->m_aspect[1],
                                                                    0, 1);
  vesMatrix4x4f view_mat = this->m_camera->ComputeViewTransform();
  vesMatrix4x4f mat = proj_mat*view_mat;
  vesVector4f world4(world[0], world[1], world[2], 1);
  vesVector4f view;
  gmtl::xform(view, mat, world4);
  view[0] /= view[3];
  view[1] /= view[3];
  view[2] /= view[3];

  // ViewToDisplay
  vesVector3f display;
  display[0] = (view[0] + 1.0f) * this->m_width / 2.0f;
  display[1] = (view[1] + 1.0f) * this->m_height / 2.0f;
  display[2] = view[2];
  return display;
}


vesVector3f vesRenderer::computeDisplayToWorld(vesVector3f display)
{
  // DisplayToView
  vesVector4f view;
  view[0] = 2.0f * display[0] / this->m_width - 1.0f;
  view[1] = 2.0f * display[1] / this->m_height - 1.0f;
  view[2] = display[2];
  view[3] = 1;

  // ViewToWorld
  vesMatrix4x4f proj_mat = this->m_camera->ComputeProjectionTransform(this->m_aspect[1],
                                                                    0, 1);
  vesMatrix4x4f view_mat = this->m_camera->ComputeViewTransform();
  vesMatrix4x4f mat = proj_mat*view_mat;
  vesMatrix4x4f inv;
  gmtl::invertFull(inv, mat);
  vesVector4f world4;
  gmtl::xform(world4, inv, view);

  vesVector3f world(world4[0]/world4[3], world4[1]/world4[3],
                    world4[2]/world4[3]);
  return world;
}


void vesRenderer::resetCamera()
{
#if 0
//  this->Actor->read();
//  this->Actor->computeBounds();
//  vesVector3f center = this->Actor->get_BBoxCenter();

  double distance;
  vesVector3f vn, vup;
  if ( this->m_camera != NULL )
  {
    vn = this->m_camera->GetViewPlaneNormal();
  }
  else
  {
    std::cout<< "Trying to reset non-existant camera" << std::endl;
    return;
  }
//  double radius = this->Actor->GetBBoxRadius();
  radius = (radius==0)?(.5):(radius);

  double angle=deg2Rad(this->m_camera->GetViewAngle());
  double parallelScale=radius;
  //this->ComputeAspect();
  if(Aspect[0]>=1.0) // horizontal window, deal with vertical angle|scale
  {
    if(this->m_camera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)/Aspect[0]);
    }
  }
  else // vertical window, deal with horizontal angle|scale
  {
    if(!this->m_camera->GetUseHorizontalViewAngle())
    {
      angle=2.0*atan(tan(angle*0.5)*Aspect[0]);
    }
    parallelScale=parallelScale/Aspect[0];
  }
  distance =radius/sin(angle*0.5);
  // check view-up vector against view plane normal
  vup = this->m_camera->GetViewUp();
  if ( fabs(gmtl::dot(vup,vn)) > 0.999 )
  {
    // vtkWarningMacro(<<"Resetting view-up since view plane normal is parallel");
    vesVector3f temp;
    temp[0] = -vup[2];
    temp[1] = vup[0];
    temp[2] = vup[1];
    this->m_camera->SetViewUp(temp);
  }
  // update the camera
  this->m_camera->SetFocalPoint(center);
  vesVector3f temp = vn;
  temp*= distance;
  temp+= center;
  this->m_camera->SetPosition(temp);

  float bounds[6];
//  bounds[0] = this->Actor->get_min()[0];
//  bounds[1] = this->Actor->get_max()[0];
//  bounds[2] = this->Actor->get_min()[1];
//  bounds[3] = this->Actor->get_max()[1];
//  bounds[4] = this->Actor->get_min()[2];
//  bounds[5] = this->Actor->get_max()[2];

  this->ResetCameraClippingRange(bounds);
  // setup default parallel scale
  this->m_camera->SetParallelScale(parallelScale);
#endif
}

void vesRenderer::resetCameraClippingRange()
{
#if 0
  this->Actor->read();
  this->Actor->computeBounds();
  float bounds[6];
  bounds[0] = this->Actor->get_min()[0];
  bounds[1] = this->Actor->get_max()[0];
  bounds[2] = this->Actor->get_min()[1];
  bounds[3] = this->Actor->get_max()[1];
  bounds[4] = this->Actor->get_min()[2];
  bounds[5] = this->Actor->get_max()[2];

  this->ResetCameraClippingRange(bounds);
#endif
}


void vesRenderer::resetCameraClippingRange(float bounds[6])
{
  vesVector3f  vn, position;
  float  a, b, c, d;
  double  range[2], dist;
  int     i, j, k;

  // Find the plane equation for the camera view plane
  vn = this->m_camera->GetViewPlaneNormal();
  position = this->m_camera->GetPosition();

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
  this->m_camera->SetClippingRange( range[0],range[1] );
}


void vesRenderer::setBackgroundColor(float r, float g, float b, float a)
{
  this->m_backgroundColor[0] = r;
  this->m_backgroundColor[1] = g;
  this->m_backgroundColor[2] = b;
  this->m_backgroundColor[3] = a;
}


void vesRenderer::setBackground(vesTexture* background)
{
//  this->Paint->setBackgroundTexture(background);
}
