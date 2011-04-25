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
#include "vtkShaderProgram.h"

#include <iostream>

vtkRenderer::vtkRenderer() : m_sphere(0), m_view(0),
  m_initialized(false), m_renderNumber(0),newFile(0)
{
  m_sphere_vbo[0] = m_sphere_vbo[1] = 0;
  m_rotation = 0;

	GLfloat matrix[16]  = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};
	currentCalculatedMatrix.m11 = (CGFloat)matrix[0];
	currentCalculatedMatrix.m12 = (CGFloat)matrix[1];
	currentCalculatedMatrix.m13 = (CGFloat)matrix[2];
	currentCalculatedMatrix.m14 = (CGFloat)matrix[3];
	currentCalculatedMatrix.m21 = (CGFloat)matrix[4];
	currentCalculatedMatrix.m22 = (CGFloat)matrix[5];
	currentCalculatedMatrix.m23 = (CGFloat)matrix[6];
	currentCalculatedMatrix.m24 = (CGFloat)matrix[7];
	currentCalculatedMatrix.m31 = (CGFloat)matrix[8];
	currentCalculatedMatrix.m32 = (CGFloat)matrix[9];
	currentCalculatedMatrix.m33 = (CGFloat)matrix[10];
	currentCalculatedMatrix.m34 = (CGFloat)matrix[11];
	currentCalculatedMatrix.m41 = (CGFloat)matrix[12];
	currentCalculatedMatrix.m42 = (CGFloat)matrix[13];
	currentCalculatedMatrix.m43 = (CGFloat)matrix[14];
	currentCalculatedMatrix.m44 = (CGFloat)matrix[15];

        _leftEye = true;
}

void vtkRenderer::readFiles(int files)
{
	//NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
	//NSString *documentsDirectory = [paths objectAtIndex:0];
	//NSString *nameOfDownloadedVTK = @"current.vtk";
	m_sphere = new vtkFileReader;
	//NSString *filePath = [documentsDirectory stringByAppendingPathComponent:nameOfDownloadedVTK];
	//m_sphere ->readFile([filePath UTF8String]);
	m_sphere ->readFile(filePath);
  _view = _view.translate(-m_sphere->center[0],-m_sphere->center[1],m_sphere->radius);
  resize(_width,_height,m_sphere->radius);
}

void vtkRenderer::resize(int width, int height, float scale)
{
  if(width > 0 && height >0){
    _width = width;
    _height = height;
  }
  const GLfloat nearp = 1, farp = 600, fov = 55*M_PI/360.0*scale;
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
  _proj.ortho(left, right, bottom, top, nearp, farp);
  //_view =  _view.translate(0,0,scale);
  glViewport(0, 0, width, height);

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);

  // Reconfigure the display params
  SetDisplay(width,height);
  SetSurfaceRotMatrix();
  SetO2ScreenLeftRightTopBottom();
  SetSurface2Base();
  SetScale(1.0);
  //CalculateProjAndView();
}

//-----------------------------------------------------------------------------
// Static Display (screen) coordinated for mobile devices.
// The z value is -0.4 meters which is the approximate distance a person holds
// the device away from the head.
void vtkRenderer::SetDisplay(int width, int height)
{
  float w = width/2;
  float h = height/2;

  _display.o[0] = -w;   _display.o[1] = -h;
  _display.x[0] =  w;   _display.x[1] = -h;
  _display.y[0] =  w;   _display.y[1] =  h;

  _display.o[2] = _display.x[2] = _display.y[2]= -0.4;
}

//-----------------------------------------------------------------------------
void vtkRenderer::SetSurfaceRotMatrix()
{
  // Base coordinates of the screen
  float xBase[3], yBase[3], zBase[3];

  xBase[0] = _display.x.X()-_display.o.X();
  xBase[1] = _display.x.Y()-_display.o.Y();
  xBase[2] = _display.x.Z()-_display.o.Z();

  yBase[0] = _display.y.X()-_display.x.X();
  yBase[1] = _display.y.Y()-_display.x.Y();
  yBase[2] = _display.y.Z()-_display.x.Z();

  Cross( xBase, yBase, zBase );

  Normalize( xBase );
  Normalize( yBase );
  Normalize( zBase );

  _surfaceRotMatrix( 0, 0) = xBase[0];
  _surfaceRotMatrix( 0, 1) = xBase[1];
  _surfaceRotMatrix( 0, 2) = xBase[2];

  _surfaceRotMatrix( 1, 0) = yBase[0];
  _surfaceRotMatrix( 1, 1) = yBase[1];
  _surfaceRotMatrix( 1, 2) = yBase[2];

  _surfaceRotMatrix( 2, 0) = zBase[0];
  _surfaceRotMatrix( 2, 1) = zBase[1];
  _surfaceRotMatrix( 2, 2) = zBase[2];
}

//------------------------------------------------------------------HeadTracked
// This enables the display config for head tracking
void vtkRenderer::SetO2ScreenLeftRightTopBottom()
{
  // Get the new DisplayOrigin, DisplayX and DisplayY after transfromation
  _display.o = _surfaceRotMatrix * _display.o;
  _display.x = _surfaceRotMatrix * _display.x;
  _display.y = _surfaceRotMatrix * _display.y;

  // Set O2Screen, O2Right, O2Left, O2Bottom, O2Top
  _O2Screen = - _display.o.Z();
  _O2Right  =   _display.x.X();
  _O2Left   = - _display.o.X();
  _O2Top    =   _display.y.Y();
  _O2Bottom = - _display.x.Y();
}

//-----------------------------------------------------------------------------
void vtkRenderer::SetSurface2Base()
{
  _surface2Base = _surfaceRotMatrix;
}

//-----------------------------------------------------------------------------
void vtkRenderer::SetScale(float scale)
{
  float interOccDist = 0.065;
  _scale = scale;
  _eyeOffset = (interOccDist*_scale)/2.0;
}

//-----------------------------------------------------------------------------
void vtkRenderer::CalculateProjAndView()
{

  vtkMatrix4f eyeOffsetMat;
  // ( _leftEye == true) ?
  //   eyeOffsetMat( 0, 3) = -_eyeOffset :
  //   eyeOffsetMat( 0, 3) =  _eyeOffset ;

  vtkMatrix4f eyePosMat = _head * eyeOffsetMat;\

  // 1. Compute Projection Matrix using head

  // Get eye position on the surface of the screen which is nothing
  // but the translation component of the eyePosMat
  vtkVector4f eyeSurface( eyePosMat( 0, 3 ),
                          eyePosMat( 1, 3 ),
                          eyePosMat( 2, 3 ),
                          0.0);

  eyeSurface = _surface2Base * eyeSurface;

  float e2Screen = ( _scale * _O2Screen ) + eyeSurface[2];
  float e2Right  = ( _scale * _O2Right )  - eyeSurface[0];
  float e2Left   = ( _scale * _O2Left )   + eyeSurface[0];
  float e2Top    = ( _scale * _O2Top )    - eyeSurface[1];
  float e2Bottom = ( _scale * _O2Bottom ) + eyeSurface[1];

  //float o2Near = _clippingRange[0] / e2Screen;
  float o2Near = 1 / e2Screen;

  // Setting the Projection matrix parameters for HeadTracked Camera
  float left   = -( e2Left   * o2Near ) ;
  float right  =   e2Right   * o2Near ;
  float top    =     e2Top   * o2Near;
  float bottom = -( e2Bottom * o2Near );

  _proj.frustum(left,right,bottom,top,1,10000.0);

  // 2. Compute View Matrix using head.
  vtkMatrix4f negEyePosMat;
  negEyePosMat = negEyePosMat.translate(-eyePosMat(0,3),
                                        -eyePosMat(1,3),
                                        -eyePosMat(2,3));
  //_view = _surface2Base * negEyePosMat;
}

void vtkRenderer::resetView()
{
  //if (m_view == 0)
    _model = vtkMatrix4f();
	GLfloat matrix[16]  = {1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0};
	currentCalculatedMatrix.m11 = (CGFloat)matrix[0];
	currentCalculatedMatrix.m12 = (CGFloat)matrix[1];
	currentCalculatedMatrix.m13 = (CGFloat)matrix[2];
	currentCalculatedMatrix.m14 = (CGFloat)matrix[3];
	currentCalculatedMatrix.m21 = (CGFloat)matrix[4];
	currentCalculatedMatrix.m22 = (CGFloat)matrix[5];
	currentCalculatedMatrix.m23 = (CGFloat)matrix[6];
	currentCalculatedMatrix.m24 = (CGFloat)matrix[7];
	currentCalculatedMatrix.m31 = (CGFloat)matrix[8];
	currentCalculatedMatrix.m32 = (CGFloat)matrix[9];
	currentCalculatedMatrix.m33 = (CGFloat)matrix[10];
	currentCalculatedMatrix.m34 = (CGFloat)matrix[11];
	currentCalculatedMatrix.m41 = (CGFloat)matrix[12];
	currentCalculatedMatrix.m42 = (CGFloat)matrix[13];
	currentCalculatedMatrix.m43 = (CGFloat)matrix[14];
	currentCalculatedMatrix.m44 = (CGFloat)matrix[15];
  //else
   // m_mv_matrix2 = m_mv_matrix2.rotate(270, vtkVector3f(1.0, 0.0, 0.0));

}

void vtkRenderer::release()
{
  // Release our VBOs before the context is destroyed
  if (m_initialized) {
    glDeleteBuffers(2, m_sphere_vbo);
  }
}

void vtkRenderer::Render(float xRotation, float yRotation, float scaleFactor, float xTranslation, float yTranslation)
{
	// Scale the view to fit current multitouch scaling
	 currentCalculatedMatrix = CATransform3DScale(currentCalculatedMatrix, scaleFactor, scaleFactor, scaleFactor);

	 // Perform incremental rotation based on current angles in X and Y
	 GLfloat totalRotation = sqrt(xRotation*xRotation + yRotation*yRotation);

	 CATransform3D temporaryMatrix = CATransform3DRotate(currentCalculatedMatrix, totalRotation * (22/7)* 1 / 180.0,
	 ((xRotation/totalRotation) * currentCalculatedMatrix.m12 + (yRotation/totalRotation) * currentCalculatedMatrix.m11),
	 ((xRotation/totalRotation) * currentCalculatedMatrix.m22 + (yRotation/totalRotation) * currentCalculatedMatrix.m21),
	 ((xRotation/totalRotation) * currentCalculatedMatrix.m32 + (yRotation/totalRotation) * currentCalculatedMatrix.m31));

	if ((temporaryMatrix.m11 >= -100.0) && (temporaryMatrix.m11 <= 100.0))
	 currentCalculatedMatrix = temporaryMatrix;

	 // Translate the model by the accumulated amount
	 float currentScaleFactor = sqrt(pow(currentCalculatedMatrix.m11, 2.0f) + pow(currentCalculatedMatrix.m12, 2.0f) + pow(currentCalculatedMatrix.m13, 2.0f));

	 xTranslation = xTranslation / (currentScaleFactor * currentScaleFactor);
	 yTranslation = yTranslation / (currentScaleFactor * currentScaleFactor);
	 // Use the (0,4,8) components to figure the eye's X axis in the model coordinate system, translate along that
	 temporaryMatrix = CATransform3DTranslate(currentCalculatedMatrix, xTranslation * currentCalculatedMatrix.m11, xTranslation * currentCalculatedMatrix.m21, xTranslation * currentCalculatedMatrix.m31);
	 // Use the (1,5,9) components to figure the eye's Y axis in the model coordinate system, translate along that
	 temporaryMatrix = CATransform3DTranslate(temporaryMatrix, yTranslation * currentCalculatedMatrix.m12, yTranslation * currentCalculatedMatrix.m22, yTranslation * currentCalculatedMatrix.m32);

	 if ((temporaryMatrix.m11 >= -100.0) && (temporaryMatrix.m11 <= 100.0))
	 currentCalculatedMatrix = temporaryMatrix;

	 // Finally, set the new matrix that has been calculated from the Core Animation transform
	 //[self convert3DTransform:&currentCalculatedMatrix toMatrix:_model.Data];

#if 1
	_model.Data[0] = (GLfloat)currentCalculatedMatrix.m11;
	_model.Data[1] = (GLfloat)currentCalculatedMatrix.m12;
	_model.Data[2] = (GLfloat)currentCalculatedMatrix.m13;
	_model.Data[3] = (GLfloat)currentCalculatedMatrix.m14;
	_model.Data[4] = (GLfloat)currentCalculatedMatrix.m21;
	_model.Data[5] = (GLfloat)currentCalculatedMatrix.m22;
	_model.Data[6] = (GLfloat)currentCalculatedMatrix.m23;
	_model.Data[7] = (GLfloat)currentCalculatedMatrix.m24;
	_model.Data[8] = (GLfloat)currentCalculatedMatrix.m31;
	_model.Data[9] = (GLfloat)currentCalculatedMatrix.m32;
	_model.Data[10] = (GLfloat)currentCalculatedMatrix.m33;
	_model.Data[11] = (GLfloat)currentCalculatedMatrix.m34;
	_model.Data[12] = (GLfloat)currentCalculatedMatrix.m41;
	_model.Data[13] = (GLfloat)currentCalculatedMatrix.m42;
	_model.Data[14] = (GLfloat)currentCalculatedMatrix.m43;
	_model.Data[15] = (GLfloat)currentCalculatedMatrix.m44;
#else
	_model.Data[0] = (GLfloat)currentCalculatedMatrix.m11;
	_model.Data[4] = (GLfloat)currentCalculatedMatrix.m12;
	_model.Data[8] = (GLfloat)currentCalculatedMatrix.m13;
	_model.Data[12] = 0(GLfloat)currentCalculatedMatrix.m14;
	_model.Data[1] = (GLfloat)currentCalculatedMatrix.m21;
	_model.Data[5] = (GLfloat)currentCalculatedMatrix.m22;
	_model.Data[9] = (GLfloat)currentCalculatedMatrix.m23;
	_model.Data[13] = 0(GLfloat)currentCalculatedMatrix.m24;
	_model.Data[2] = (GLfloat)currentCalculatedMatrix.m31;
	_model.Data[6] = (GLfloat)currentCalculatedMatrix.m32;
	_model.Data[10] = 0(GLfloat)currentCalculatedMatrix.m33;
	_model.Data[14] = (GLfloat)currentCalculatedMatrix.m34;
	_model.Data[3] = (GLfloat)currentCalculatedMatrix.m41;
	_model.Data[7] = (GLfloat)currentCalculatedMatrix.m42;
	_model.Data[11] = (GLfloat)currentCalculatedMatrix.m43;
	_model.Data[15] = 1(GLfloat)currentCalculatedMatrix.m44;
#endif
}

void vtkRenderer::Render()
{
	//if(!newFile) return;
  if (!m_initialized) {
      if (m_renderNumber == 2000000000) {
		  readFiles(0);
		  glGenBuffers(2, m_sphere_vbo);
		  glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo[0]);
		  glBufferData(GL_ARRAY_BUFFER, m_sphere->m_numPoints * 6 * sizeof(float),
					   m_sphere->m_points, GL_STATIC_DRAW);
		  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphere_vbo[1]);
		  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere->m_numTriangles * 3 * sizeof(unsigned short),
					   m_sphere->m_triangles, GL_STATIC_DRAW);

		  glBindBuffer(GL_ARRAY_BUFFER, 0);
		  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		  m_initialized = true;
		  newFile=0;
	  }
	  ++m_renderNumber;
  }

	if (newFile) {
		readFiles(2);
		glGenBuffers(2, m_sphere_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo[0]);
		glBufferData(GL_ARRAY_BUFFER, m_sphere->m_numPoints * 6 * sizeof(float),
					 m_sphere->m_points, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_sphere_vbo[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere->m_numTriangles * 3 * sizeof(unsigned short),
					 m_sphere->m_triangles, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		m_initialized = true;
		newFile =0;
	}

  // Work out the appropriate matrices
  vtkMatrix4f mvp;
  vtkMatrix3f normal_matrix(mvp);
  vtkVector4f lightDir;
  mvp = _proj * _view * _model;
	vtkMatrix4f temp = _model.transpose();
	temp.normalized();
	lightDir = temp*vtkMatrix4f()*vtkVector4f(0.0,0.0,.650,0.0);
	
  vtkVector3f light(lightDir.X(),lightDir.Y(),lightDir.Z());
  glUniformMatrix4fv(this->Program->GetUniform("u_mvpMatrix"), 1, GL_FALSE, mvp.Data);
  glUniformMatrix3fv(this->Program->GetUniform("u_normalMatrix"), 1, GL_FALSE, normal_matrix.Data);
  glUniform3fv      (this->Program->GetUniform("u_ecLightDir"), 1, light.Data);

  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Enable our attribute arrays
  glEnableVertexAttribArray(this->Program->GetAttribute("a_vertex"));
  glEnableVertexAttribArray(this->Program->GetAttribute("a_normal"));

  if (m_sphere) {
    glVertexAttrib4f(this->Program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
    glVertexAttribPointer(this->Program->GetAttribute("a_vertex"), 3, GL_FLOAT, 0, 6 * sizeof(float), m_sphere->m_points);
    glVertexAttribPointer(this->Program->GetAttribute("a_normal"), 3, GL_FLOAT, 0, 6 * sizeof(float), m_sphere->m_points[0].normal.GetData());

    // Draw
    glDrawElements(GL_TRIANGLES, m_sphere->m_numTriangles * 3, GL_UNSIGNED_SHORT, m_sphere->m_triangles);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(this->Program->GetAttribute("a_vertex"));
  glDisableVertexAttribArray(this->Program->GetAttribute("a_normal"));
}
