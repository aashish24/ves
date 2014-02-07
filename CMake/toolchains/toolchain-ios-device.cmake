set(CMAKE_SYSTEM_NAME Darwin)

exec_program(/usr/bin/xcode-select ARGS -print-path OUTPUT_VARIABLE XCODE_DEVELOPER_DIR)

find_program(CMAKE_C_COMPILER NAME gcc
  PATHS
  "${XCODE_DEVELOPER_DIR}/Platforms/iPhoneOS.platform/Developer/usr/bin/"
  /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/
  /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/
  /Applications/Xcode.app/Contents/Developer/usr/bin
  NO_DEFAULT_PATH)

find_program(CMAKE_CXX_COMPILER NAME g++
  PATHS
  "${XCODE_DEVELOPER_DIR}/Platforms/iPhoneOS.platform/Developer/usr/bin/"
  /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/
  /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/
 /Applications/Xcode.app/Contents/Developer/usr/bin
  NO_DEFAULT_PATH)

set(CMAKE_OSX_ARCHITECTURES "armv7;armv7s")

# Set the CMAKE_OSX_SYSROOT to the latest SDK found
set(CMAKE_OSX_SYSROOT)
set(possible_sdk_roots
  "${XCODE_DEVELOPER_DIR}/Platforms/iPhoneOS.platform/Developer/SDKs"
  /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs
  /Developer/Platforms/iPhoneOS.platform/Developer/SDKs
  )
foreach(sdk_root ${possible_sdk_roots})
  foreach(sdk iPhoneOS4.3.sdk iPhoneOS5.0.sdk iPhoneOS5.1.sdk iPhoneOS6.0.sdk iPhoneOS6.1.sdk iPhoneOS7.0.sdk)
    if (EXISTS ${sdk_root}/${sdk} AND IS_DIRECTORY ${sdk_root}/${sdk})
      set(CMAKE_OSX_SYSROOT ${sdk_root}/${sdk})
    endif()
  endforeach()
endforeach()
if (NOT CMAKE_OSX_SYSROOT)
  message(FATAL_ERROR "Could not find a usable iOS SDK in ${sdk_root}")
endif()
message(STATUS "-- Using iOS SDK: ${CMAKE_OSX_SYSROOT}")

set(ves_ios_cmake_cxx_flags)
set(ves_ios_cmake_c_flags)

# Use appropritate flags
if(CMAKE_OSX_SYSROOT MATCHES iPhoneOS7.0.sdk)
  message(STATUS "-- running compiler ${CMAKE_CXX_COMPILER}")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
    OUTPUT_VARIABLE out
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  message(STATUS "--- output from compiler is ${out}")
  string(REGEX REPLACE ".*clang-(.).*" "\\1" CLANG_VERSION "${out}")
  if("${CLANG_VERSION}" GREATER 4)
     message(STATUS "CLANG using version 7")
    set(ves_ios_cmake_cxx_flags "-miphoneos-version-min=7.0")
    set(ves_ios_cmake_c_flags   "-miphoneos-version-min=7.0")
  else()
    set(ves_ios_cmake_cxx_flags "-miphoneos-version-min=5.0")
    set(ves_ios_cmake_c_flags "-miphoneos-version-min=5.0")
  endif()
else()
  message("testing compiler version: ${CMAKE_OSX_SYSROOT}")
  set(ves_ios_cmake_cxx_flags "-miphoneos-version-min=5.0")
  set(ves_ios_cmake_c_flags "-miphoneos-version-min=5.0")
endif()

message(STATUS "ves ios cxx flag is ${ves_ios_cmake_cxx_flags}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ves_ios_cmake_cxx_flags} -fvisibility=hidden -fvisibility-inlines-hidden")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ves_ios_cmake_c_flags} -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}" CACHE STRING "osx architectures")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}" CACHE PATH "osx sysroot")
set(MACOSX_BUNDLE_GUI_IDENTIFIER CACHE STRING "com.kitware.\${PRODUCT_NAME:identifier}")


set(CMAKE_FIND_ROOT_PATH ${CMAKE_OSX_SYSROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
