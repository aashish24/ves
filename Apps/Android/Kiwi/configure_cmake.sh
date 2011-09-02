#!/bin/bash

source tools.sh

VTK_DIR=../../CMakeBuild/build/CMakeExternals/Build/vtkmodular-android
VES_DIR=../../CMakeBuild/build/CMakeExternals/Build/ves-android
TOOLCHAIN=../../CMakeBuild/cmake/android.toolchain.cmake

mkdir -p build
cd build
$CMAKE -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN -DANDROID_LEVEL=9 -DVTK_DIR=$VTK_DIR -DVES_DIR=$VES_DIR ../
