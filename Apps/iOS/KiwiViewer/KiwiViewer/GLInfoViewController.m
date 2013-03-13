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

#import "GLInfoViewController.h"

#import <GLKit/GLKit.h>

#include <vector>
#include <string>
#include <sstream>

@interface GLInfoViewController () {

  std::vector<std::string> fieldNames;
  std::vector<std::string> fieldValues;

  std::vector<std::string> extensions;
}

@end

@implementation GLInfoViewController

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

-(std::string)getStr:(int) value
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

-(std::string)doubleStr:(double) value
{
  std::stringstream ss;
  ss << value;
  return ss.str();
}

- (void)viewDidLoad
{
  [super viewDidLoad];
  self.tableView.allowsSelection = NO;



  self->fieldNames.push_back("GL_VENDOR");
  self->fieldValues.push_back(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));

  self->fieldNames.push_back("GL_RENDERER");
  self->fieldValues.push_back(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

  self->fieldNames.push_back("GL_VERSION");
  self->fieldValues.push_back(reinterpret_cast<const char*>(glGetString(GL_VERSION)));

  self->fieldNames.push_back("GL_SHADING_LANGUAGE_VERSION");
  self->fieldValues.push_back(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

  int maxTextureSize;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
  self->fieldNames.push_back("GL_MAX_TEXTURE_SIZE");
  self->fieldValues.push_back([self getStr:maxTextureSize]);


  // Set extension list
  std::stringstream ext;
  std::string str;

  const GLubyte* exts = glGetString(GL_EXTENSIONS);

  ext << exts;
  while (ext >> str) {
    self->extensions.push_back(str);
  }

}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
  return 2;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
  if (section == 0) {
    return self->fieldNames.size();
  }
  else {
    return self->extensions.size();
  }
}

-(NSString*)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
  if (section == 0) {
    return @"Info";
  }
  else {
    return @"Extensions";
  }
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
  if (indexPath.section == 0) {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleSubtitle reuseIdentifier:@"InfoCell"];
    cell.textLabel.text = [NSString stringWithUTF8String:self->fieldNames[indexPath.row].c_str()];
    cell.detailTextLabel.text = [NSString stringWithUTF8String:self->fieldValues[indexPath.row].c_str()];
    return cell;
  }
  else {
    UITableViewCell *cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault reuseIdentifier:@"ExtensionCell"];
    cell.textLabel.text = [NSString stringWithUTF8String:self->extensions[indexPath.row].c_str()];
    return cell;
  }
  
}

@end
