cmake_minimum_required(VERSION 2.8)
project(KiwiViewer C CXX)

# Work around bug in CMakeFindEclipseCDT4.cmake since this is a cross
# compile UNIX is set, but make program is nmake which does not
# take -jN...
if( "${CMAKE_MAKE_PROGRAM}" MATCHES nmake)
  set(CMAKE_ECLIPSE_MAKE_ARGUMENTS "")
endif()

if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "Kiwi does not support in-source builds :) .")
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs/${ARMEABI_NDK_NAME})
add_subdirectory(jni)

include(assets.cmake)

find_program(ANT_EXECUTABLE
  NAMES ant
  DOC   "The ant build tool")
if(NOT ANT_EXECUTABLE)
  message(FATAL_ERROR "Can not find ant build tool: ant")
endif()

find_program(ANDROID_EXECUTABLE
  NAMES android
  DOC   "The android command-line tool")
if(NOT ANDROID_EXECUTABLE)
  message(FATAL_ERROR "Can not find android command line tool: android")
endif()

set(ANDROID_TARGET "android-18"
  CACHE STRING "The Android API to target")

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/ant.properties.in"
  "${CMAKE_CURRENT_BINARY_DIR}/ant.properties"
  @ONLY)
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/AndroidManifest.xml"
  "${CMAKE_CURRENT_BINARY_DIR}/AndroidManifest.xml"
  COPYONLY)

add_custom_target(kiwi-ant-configure ALL
  COMMAND "${ANDROID_EXECUTABLE}"
          update project
          --name KiwiViewer
          --path "${CMAKE_CURRENT_SOURCE_DIR}"
          --target "${ANDROID_TARGET}"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
          "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/local.properties"
          "${CMAKE_CURRENT_BINARY_DIR}/local.properties"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/project.properties"
          "${CMAKE_CURRENT_BINARY_DIR}/project.properties"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different
          "${CMAKE_CURRENT_SOURCE_DIR}/proguard-project.txt"
          "${CMAKE_CURRENT_BINARY_DIR}/proguard-project.txt"
  COMMAND "${CMAKE_COMMAND}" -E remove
          "${CMAKE_CURRENT_SOURCE_DIR}/build.xml"
          "${CMAKE_CURRENT_SOURCE_DIR}/local.properties"
          "${CMAKE_CURRENT_SOURCE_DIR}/project.properties"
          "${CMAKE_CURRENT_SOURCE_DIR}/proguard-project.txt"
  WORKING_DIRECTORY
          "${CMAKE_CURRENT_BINARY_DIR}")
add_dependencies(kiwi-ant-configure
  KiwiNative)

# For Android-Eclipse we need to copy resource dir to binary dir
# or else it will not find resources.

if("${CMAKE_EXTRA_GENERATOR}" STREQUAL "Eclipse CDT4")
  # Remove existing resource directory if any in the binary dir
  add_custom_target(kiwi-apk-rm-resource ALL
    COMMAND ${CMAKE_COMMAND} -E remove_directory
            "${CMAKE_CURRENT_BINARY_DIR}/res")
  add_dependencies(kiwi-apk-rm-resource
    kiwi-ant-configure
    KiwiNative)

  # Copy res from the source dir to binary dir
  add_custom_target(kiwi-apk-resource ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/res"
            "${CMAKE_CURRENT_BINARY_DIR}/res")
  add_dependencies(kiwi-apk-resource
    kiwi-apk-rm-resource
    kiwi-ant-configure
    KiwiNative)
endif()

add_custom_target(kiwi-apk-release ALL
  COMMAND ${ANT_EXECUTABLE}
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          release)
add_dependencies(kiwi-apk-release
  kiwi-ant-configure
  KiwiNative
  kiwi-apk-resource)

add_custom_target(kiwi-apk-debug ALL
  COMMAND ${ANT_EXECUTABLE}
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          debug)
add_dependencies(kiwi-apk-debug
  kiwi-apk-release
  kiwi-ant-configure
  KiwiNative
  kiwi-apk-resource)
