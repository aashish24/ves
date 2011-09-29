set(CMAKE_SYSTEM_NAME Darwin)

set(CMAKE_C_COMPILER /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/gcc)
set(CMAKE_CXX_COMPILER /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/g++)

set(CMAKE_OSX_ARCHITECTURES armv7)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0 -pipe")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-trigraphs -fpascal-strings")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wreturn-type -Wunused-variable")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -mmacosx-version-min=10.6")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2 -fvisibility=hidden")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-abi-version=2 -fobjc-legacy-dispatch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPHONE_OS_VERSION_MIN_REQUIRED=40300")
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_OSX_SYSROOT /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk)

set(CMAKE_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}" CACHE STRING "osx architectures")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}" CACHE PATH "osx sysroot")
set(MACOSX_BUNDLE_GUI_IDENTIFIER CACHE STRING "com.kitware.\${PRODUCT_NAME:identifier}")


#-isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk

set(CMAKE_FIND_ROOT_PATH /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
