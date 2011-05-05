/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    vtkFileReader.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#ifndef vtkFBO_H
#define vtkFBO_H

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

class vtkFBO
{
public:
  vtkFBO();
  ~vtkFBO();
  void createFrameBuffer();
  void deleteFrameBuffer();
};
#endif //vtkFBO_H