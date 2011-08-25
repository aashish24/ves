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

#import <Foundation/Foundation.h>

struct vesTriangleData;

@interface DataReader : NSObject {
  NSString *errorTitle;
  NSString *errorMessage;
}

@property (nonatomic, retain) NSString* errorTitle;
@property (nonatomic, retain) NSString* errorMessage;

// Description:
// Creates an appropriate reader for the given filename and reads the data from storage.
// If there is an error, returns NULL and sets the errorTitle and errorMessage properties.
// The caller takes ownership of the returned pointer.
- (struct vesTriangleData*) readData:(NSString*)filename;

@end
