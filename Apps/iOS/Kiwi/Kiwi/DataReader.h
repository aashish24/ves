//
//  DataReader.h
//  kiwi
//
//  Created by pat on 7/9/11.
//  Copyright 2011 Kitware. All rights reserved.
//

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