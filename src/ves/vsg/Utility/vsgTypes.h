// ============================================================================
/**
 * @file   vsgTypes.h
 *
 * @section COPYRIGHT
 *
 * Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 * All rights reserved.
 * See Copyright.txt or http://www.kitware.com/Copyright.htm for details.
 *
 *   This software is distributed WITHOUT ANY WARRANTY; without even
 *   the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *   PURPOSE.  See the above copyright notice for more information.
 *
 * @section DESCRIPTION
 *
 * This file describes all the types defined in VTK Scene Graph
 *
 * @author nikhil shetty <nikhil.shetty@kitware.com>
 */
// ============================================================================
#ifndef __vsgTypes_h
#define __vsgTypes_h
// --------------------------------------------------------------------includes
# include "vesGMTL.h"
# include <vector>
#include <sys/time.h>

typedef bool SFBool;
typedef gmtl::Vec3f SFColor;
typedef gmtl::Vec4f SFColorRGBA;
typedef double SFDouble;
typedef float SFFloat;
typedef struct {
  unsigned int width;
  unsigned int height;
  enum{ONE=1,TWO=2,THREE=3,FOUR=4} component;
  void *data;
} SFImage;
typedef unsigned int SFInt32;
typedef unsigned short SFInt16;
typedef unsigned char SFInt8;
typedef gmtl::Matrix<double,3,3> SFMatrix3d;
typedef gmtl::Matrix<float,3,3> SFMatrix3f;
typedef gmtl::Matrix<double,4,4> SFMatrix4d;
typedef gmtl::Matrix<float,4,4> SFMatrix4f;
typedef gmtl::Vec4f SFRotation;
typedef std::string SFString;
typedef struct timeval SFTime;
typedef gmtl::Vec2f SFVec2f;
typedef gmtl::Vec2d SFVec2d;
typedef gmtl::Vec3f SFVec3f;
typedef gmtl::Vec3d SFVec3d;
typedef gmtl::Vec4f SFVec4f;
typedef gmtl::Vec4d SFVec4d;

typedef std::vector<bool> MFBool;
typedef std::vector<SFColor> MFColor;
typedef std::vector<SFColorRGBA> MFColorRGBA;
typedef std::vector<SFDouble> MFDouble;
typedef std::vector<SFFloat> MFFloat;
typedef std::vector<SFImage> MFImage;
typedef std::vector<SFInt32> MFInt32;
typedef std::vector<SFInt16> MFInt16;
typedef std::vector<SFInt8> MFInt8;
typedef std::vector<SFMatrix3d> MFMatrix3d;
typedef std::vector<SFMatrix3f> MFMatrix3f;
typedef std::vector<SFMatrix4d> MFMatrix4d;
typedef std::vector<SFMatrix4f> MFMatrix4f;
typedef std::vector<SFRotation> MFRotation;
typedef std::vector<SFString> MFString;
typedef struct std::vector<SFTime> MFTime;
typedef std::vector<SFVec2d> MFVec2d;
typedef std::vector<SFVec2f> MFVec2f;
typedef std::vector<SFVec2d> MFVec2d;
typedef std::vector<SFVec2f> MFVec2f;
typedef std::vector<SFVec3d> MFVec3d;
typedef std::vector<SFVec3f> MFVec3f;
typedef std::vector<SFVec4d> MFVec4d;
typedef std::vector<SFVec4f> MFVec4f;
#endif // __vsgTypes_h
