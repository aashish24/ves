/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ES2Renderer.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import "ESRenderer.h"

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

struct vesRenderer;
struct vesShaderProgram;
struct vesShader;
struct vesMapper;
struct vesActor;
struct vesCamera;

@interface ES2Renderer : NSObject <ESRenderer>
{
@private

  vesShader *Shader;
  vesShaderProgram *shaderProgram;
	vesRenderer *renderer;
  vesMapper* mMapper;
  vesActor* mActor;
}

- (struct vesRenderer*) getRenderer;
- (struct vesShader*) getShader;
- (struct vesCamera*) getCamera;

- (void)render;
- (void)resetView;
- (BOOL)resizeFromLayer:(int)w height:(int) h;
- (void)setFilePath:(NSString*)fpath;

- (int)getNumberOfFacetsForCurrentModel;
- (int)getNumberOfLinesForCurrentModel;
- (int)getNumberOfVerticesForCurrentModel;
@end

