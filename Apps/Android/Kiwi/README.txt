The Android NDK is required to build Kiwi. It is available from
https://developer.android.com/tools/sdk/ndk/index.html. A cross-compiled VTK
and VES are required for Kiwi. Build the Apps/Android/CMakeBuild directory:

    export ANDROID_NDK="path/to/ndk/root"
    cmake -DVES_ANDROID_SUPERBUILD:BOOL=TRUE "$ves_src/Apps/Android/CMakeBuild"
    # Build the project.

After it is build, Kiwi must be pointed at these cross-compiled builds:

    cmake \
      -DCMAKE_TOOLCHAIN_FILE="$ves_src/CMake/toolchains/android.toolchain.cmake" \
      -DANDROID_NATIVE_API_LEVEL=8 \
      -DVTK_DIR="path/to/cross/build/CMakeExternals/Build/vtk-android" \
      -DVES_DIR="path/to/cross/build/CMakeExternals/Build/ves-android" \
      "$ves_src/Apps/Android/Kiwi"
    # Build the project.

This will create APK files in the bin directory.
