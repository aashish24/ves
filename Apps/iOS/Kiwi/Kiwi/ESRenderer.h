/*=========================================================================
 
 Program:   Visualization Toolkit
 Module:    ESRenderer.h
 
 Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 All rights reserved.
 See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 
 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.
 
 =========================================================================*/

#import <QuartzCore/QuartzCore.h>

#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>

@protocol ESRenderer <NSObject>

- (void)render;
- (void)resetView;
- (BOOL)resizeFromLayer:(int)w height:(int) h;
- (void)setFilePath:(NSString*)fpath;
- (void)_drawViewByRotatingAroundX:(float)xRotation 
                   rotatingAroundY:(float)yRotation 
                           scaling:(float)scaleFactor 
                    translationInX:(float)xTranslation 
                    translationInY:(float)yTranslation;
- (int)getNumberOfFacetsForCurrentModel;
- (int)getNumberOfLinesForCurrentModel;
- (int)getNumberOfVerticesForCurrentModel;

@end
