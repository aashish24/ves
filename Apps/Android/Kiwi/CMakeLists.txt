cmake_minimum_required(VERSION 2.8)
project(KiwiViewer)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/libs/${ARMEABI_NDK_NAME})
add_subdirectory(jni)

include(assets.cmake)
