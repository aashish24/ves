/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkProgramShader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkProgramShader - Defines a collection of individual shaders.
// .SECTION Description
// vtkProgramShader [refer 31.4.6]

#ifndef __vtkProgramShader_h
#define __vtkProgramShader_h
// --------------------------------------------------------------------includes
#include "vtkShaderNode.h"
#include "vtkSetGet.h"
#include <vector>

// -----------------------------------------------------------------pre-defines
class vtkProgramShaderInternal;
class vtkShaderProgram;

// -----------------------------------------------------------------------class
class vtkProgramShader : public vtkShaderNode
{
public:
  // ............................................................public-methods
  vtkProgramShader();
  ~vtkProgramShader();
  
  void SetPrograms(const std::vector<vtkShaderProgram*> &_arg)
  {                                             
    for(int i =0; i< _arg.size(); i++)          
    {                                         
      this->Programs.push_back( _arg[i]);                
    }                                         
  } 
  
  bool GetPrograms (std::vector<vtkShaderProgram*> &_arg)      
  {                                             
    if(this->Programs.size() ==0)                   
      return false;                             
    for(int i=0; i < this->Programs.size(); i++) 
    {                                         
      _arg.push_back( this->Programs[i]);              
    }                                         
    return true;                                
  }
  
//#define vtkSetGetVectorMacro(name,type)         \
//void Set##name(const std::vector<type> &_arg) \
//{                                             \
//for(int i =0; i< _arg.size(); i++)          \
//{                                         \
//this->name[i] = _arg[i];                \
//}                                         \
//}                                             \
//\
//bool Get##name (std::vector<type> &_arg)      \
//{                                             \
//if(this->name.size() ==0)                   \
//return false;                             \
//for(int ii=0; ii < this->name.size(); ii++) \
//{                                         \
//_arg[ii] = this->name[ii];              \
//}                                         \
//return true;                                \
//}
//
protected:
  // ...........................................................protected-ivars
  std::vector<vtkShaderProgram*> Programs;
protected:
//BTX
  // .......................................................................BTX

private:
  vtkProgramShaderInternal *Internal;

//ETX
  // .......................................................................ETX
};

#endif // __vtkProgramShader_h
