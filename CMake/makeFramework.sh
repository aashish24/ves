#!/bin/bash

#------------------------------------------------------------------------------
install=./CMakeExternals/Install
if [ ! -d $install ]; then
  echo "Install directory not found.  This script should be run from the top level build directory that contains CMakeExternals/Install."
  exit 1
fi

#------------------------------------------------------------------------------
make_kiwi_framework ()
{
  if [ -d $install/libarchive-ios-device/lib ]; then
    device_archive_libs=`ls $install/libarchive-ios-device/lib/*.a`
  fi
  if [ -d $install/curl-ios-device/lib ]; then
    device_curl_libs=`ls $install/curl-ios-device/lib/*.a`
  fi
  device_libs=`ls $install/ves-ios-device/lib/*.a $device_archive_libs $device_curl_libs`

  if [ -d $install/libarchive-ios-simulator/lib ]; then
    sim_archive_libs=`ls $install/libarchive-ios-simulator/lib/*.a`
  fi
  if [ -d $install/curl-ios-simulator/lib ]; then
    sim_curl_libs=`ls $install/curl-ios-simulator/lib/*.a`
  fi
  sim_libs=`ls $install/ves-ios-simulator/lib/*.a $sim_archive_libs $sim_curl_libs`

  headers=`find $install/ves-ios-device/include -name \\*.h`
  headers="$headers $install/eigen/Eigen"

  libname=kiwi
  framework=$install/frameworks/kiwi.framework

  mkdir -p $framework
  rm -rf $framework/*
  mkdir $framework/Headers
  cp -r $headers $framework/Headers/

  libtool -static -o $framework/lib_device $device_libs
  libtool -static -o $framework/lib_sim $sim_libs
  lipo -create $framework/lib_device $framework/lib_sim -output $framework/$libname
  rm $framework/lib_device $framework/lib_sim
}


#------------------------------------------------------------------------------
make_vtk_framework ()
{
  device_libs=`ls $install/vtk-ios-device/lib/*.a`
  sim_libs=`ls $install/vtk-ios-simulator/lib/*.a`
  headers=$install/vtk-ios-device/include/vtk-6.0

  libname=vtk
  framework=$install/frameworks/vtk.framework

  mkdir -p $framework
  rm -rf $framework/*
  mkdir $framework/Headers
  cp -r $headers/* $framework/Headers/

  libtool -static -o $framework/lib_device $device_libs
  libtool -static -o $framework/lib_sim $sim_libs
  lipo -create $framework/lib_device $framework/lib_sim -output $framework/$libname
  rm $framework/lib_device $framework/lib_sim
}

#------------------------------------------------------------------------------
if [ "$1" == "kiwi" ]; then
  make_kiwi_framework
elif [ "$1" == "vtk" ]; then
  make_vtk_framework
else
  echo "Usage: $0 kiwi|vtk"
  exit 1
fi
