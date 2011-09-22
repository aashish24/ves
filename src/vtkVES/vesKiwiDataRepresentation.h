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

#ifndef __vesKiwiDataRepresentation_h
#define __vesKiwiDataRepresentation_h

class vesTriangleData;
class vesActor;
class vesMapper;
class vesRenderer;
class vesShaderProgram;

class vtkDataSet;

class vesKiwiDataRepresentation
{
public:

  vesKiwiDataRepresentation();
  ~vesKiwiDataRepresentation();

  void initializeWithShader(vesShaderProgram* shaderProgram);

  void setDataSet(vtkDataSet* data);
  vesTriangleData* triangleData() const;

  void addSelfToRenderer(vesRenderer* renderer);
  void removeSelfFromRenderer(vesRenderer* renderer);

protected:

  vesActor* actor() const;
  vesMapper* mapper() const;

private:

  vesKiwiDataRepresentation(const vesKiwiDataRepresentation&); // Not implemented
  void operator=(const vesKiwiDataRepresentation&); // Not implemented

  class vesInternal;
  vesInternal* Internal;
};


#endif
