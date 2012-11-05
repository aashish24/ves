@echo off

call ".\tools.bat"
set VTK_DIR=%cmakeexternals%/Build/vtk-android
set VES_DIR=%cmakeexternals%/Build/ves-android
set TOOLCHAIN=%source_dir%/CMake/toolchains/android.toolchain.cmake

mkdir %build_dir%
cd %build_dir%
%CMAKE% -DCMAKE_TOOLCHAIN_FILE=%TOOLCHAIN% -DANDROID_NATIVE_API_LEVEL=8 -DVTK_DIR=%VTK_DIR% -DVES_DIR=%VES_DIR% %app_dir% -G "NMake Makefiles"
