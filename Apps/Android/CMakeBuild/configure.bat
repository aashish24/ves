@echo off

rem Set the NDK path here
set ANDROID_NDK=c:/tools/android-ndk-r8

rem set ANDROID_TOOLCHAIN_NAME=arm-linux-androideabi-4.4.3
set CMAKE_DEFAULT_GENERATOR="NMake Makefiles"
set BUILD_TYPE=Release

rem set CMAKE_HOST_WIN32=1
set CL=/MP

set build_dir=%CD%\build
set source_dir=%CD%\..\..\..

echo Android NDK directory: %ANDROID_NDK%
echo Build type: %BUILD_TYPE%

cmake.exe -E make_directory "%build_dir%" 
cd "%build_dir%"
cmake.exe -G %CMAKE_DEFAULT_GENERATOR%  -DCMAKE_BUILD_TYPE:STRING=%BUILD_TYPE% -DVES_ANDROID_SUPERBUILD:BOOL=TRUE "%source_dir%" 
cd ..

echo Configuration done.
echo To build VES, go to the build directory and type nmake.
echo Don't forget to add tools and platforms directory of Android SDK to PATH env var.