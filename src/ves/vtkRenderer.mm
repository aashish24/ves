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
}

void vtkRenderer::readFiles(int files)
{
	m_sphere = new vtkFileReader;
	m_sphere ->readFile(filePath);
  _view = makeTranslationMatrix4x4(vtkVector3f(m_sphere->center[0],m_sphere->center[1],m_sphere->radius));
  _view = makeScaleMatrix4x4(1/m_sphere->radius)*_view;
  resize(_width,_height,m_sphere->radius);
}

void vtkRenderer::resize(int width, int height, float scale)
{
  if(width > 0 && height >0){
    _width = width;
    _height = height;
  }
  const GLfloat nearp = .7, farp = 5, fov = 60*M_PI/360.0*scale*.5;
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
  
  glViewport(0, 0, width, height);

  glClearColor(63/255.0f, 96/255.0f, 144/255.0, 1.0f);
}

void vtkRenderer::resetView()
{
  _model = vtkMatrix4x4f();
  _nav.Reset();	
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
#if 1
  _nav.UpdateMatrix(xRotation,yRotation,scaleFactor,xTranslation,yTranslation);
 
	_model.mData[0] = (GLfloat)_nav.GetMatrix().m11;
	_model.mData[1] = (GLfloat)_nav.GetMatrix().m12;
	_model.mData[2] = (GLfloat)_nav.GetMatrix().m13;
	_model.mData[3] = (GLfloat)_nav.GetMatrix().m14;
	_model.mData[4] = (GLfloat)_nav.GetMatrix().m21;
	_model.mData[5] = (GLfloat)_nav.GetMatrix().m22;
	_model.mData[6] = (GLfloat)_nav.GetMatrix().m23;
	_model.mData[7] = (GLfloat)_nav.GetMatrix().m24;
	_model.mData[8] = (GLfloat)_nav.GetMatrix().m31;
	_model.mData[9] = (GLfloat)_nav.GetMatrix().m32;
	_model.mData[10] = (GLfloat)_nav.GetMatrix().m33;
	_model.mData[11] = (GLfloat)_nav.GetMatrix().m34;
	_model.mData[12] = (GLfloat)_nav.GetMatrix().m41;
	_model.mData[13] = (GLfloat)_nav.GetMatrix().m42;
	_model.mData[14] = (GLfloat)_nav.GetMatrix().m43;
	_model.mData[15] = (GLfloat)_nav.GetMatrix().m44;
#else
  _nav.UpdateMatrixGMTL(xRotation,yRotation,scaleFactor,xTranslation,yTranslation);
  _model = _nav.GetMatrixGMTL();
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
		  glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere->m_triangles.size() * 3 * sizeof(unsigned short),
					   &m_sphere->m_triangles[0], GL_STATIC_DRAW);

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
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_sphere->m_triangles.size() * 3 * sizeof(unsigned short),
					 &m_sphere->m_triangles[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		m_initialized = true;
		newFile =0;
	}

  // Work out the appropriate matrices
  vtkMatrix4x4f mvp;
  mvp = _proj * _view * _model;
	
  vtkMatrix3x3f normal_matrix = makeNormalMatrix3x3f(_view);
  vtkMatrix4x4f temp = makeNormalizedMatrix4x4(makeTransposeMatrix4x4(_model));
  vtkPoint3f lightDir = transformVector4f(temp,vtkPoint3f(0.0,0.0,.650));
	
  vtkVector3f light(lightDir.mData[0],lightDir.mData[1],lightDir.mData[2]);
  glUniformMatrix4fv(this->Program->GetUniform("u_mvpMatrix"), 1, GL_FALSE, mvp.mData);
  glUniformMatrix3fv(this->Program->GetUniform("u_normalMatrix"), 1, GL_FALSE, normal_matrix.mData);
  glUniform3fv      (this->Program->GetUniform("u_ecLightDir"), 1, light.mData);

  // Clear the buffers
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // Enable our attribute arrays
  glEnableVertexAttribArray(this->Program->GetAttribute("a_vertex"));
  glEnableVertexAttribArray(this->Program->GetAttribute("a_normal"));

  if (m_sphere) {
    glVertexAttrib4f(this->Program->GetAttribute("a_texcoord"), 0.8, 0.8, 0.8, 1.0);
    glVertexAttribPointer(this->Program->GetAttribute("a_vertex"), 3, GL_FLOAT, 0, 6 * sizeof(float), m_sphere->m_points);
    glVertexAttribPointer(this->Program->GetAttribute("a_normal"), 3, GL_FLOAT, 0, 6 * sizeof(float), m_sphere->m_points[0].normal.mData);

    // Draw
    glDrawElements(GL_TRIANGLES, m_sphere->m_triangles.size() * 3, GL_UNSIGNED_SHORT, &m_sphere->m_triangles[0]);
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glDisableVertexAttribArray(this->Program->GetAttribute("a_vertex"));
  glDisableVertexAttribArray(this->Program->GetAttribute("a_normal"));
}
