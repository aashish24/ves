cmake_minimum_required(VERSION 2.8)
project(KiwiViewer CXX Java)

if (CMAKE_BINARY_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  message(FATAL_ERROR "Kiwi does not support in-source builds :) .")
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs/${ARMEABI_NDK_NAME})
add_subdirectory(jni)

include(assets.cmake)

find_program(ANT_EXECUTABLE
  NAMES ant
  DOC   "The ant build tool")
find_program(ANDROID_EXECUTABLE
  NAMES android
  DOC   "The android command-line tool")

set(ANDROID_TARGET "android-16"
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

add_custom_target(kiwi-apk-release ALL
  COMMAND ant
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          release)
add_dependencies(kiwi-apk-release
  kiwi-ant-configure
  KiwiNative)
add_custom_target(kiwi-apk-debug ALL
  COMMAND ant
          -file "${CMAKE_CURRENT_BINARY_DIR}/build.xml"
          debug)
add_dependencies(kiwi-apk-debug
  kiwi-ant-configure
  KiwiNative)
