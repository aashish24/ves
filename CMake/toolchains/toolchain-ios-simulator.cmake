set(CMAKE_SYSTEM_NAME Darwin)

set(CMAKE_C_COMPILER /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/gcc)
set(CMAKE_CXX_COMPILER /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/g++)
#set(CMAKE_LINKER /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ld)
#set(CMAKE_AR /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/ar)
#set(CMAKE_STRIP /Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/strip)


set(CMAKE_OSX_ARCHITECTURES i386)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0 -pipe")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-trigraphs -fpascal-strings")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -Wreturn-type -Wunused-variable")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fasm-blocks -mmacosx-version-min=10.6")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -gdwarf-2 -fvisibility=hidden")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility-inlines-hidden")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-abi-version=2 -fobjc-legacy-dispatch")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__IPHONE_OS_VERSION_MIN_REQUIRED=40300")
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_OSX_SYSROOT /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.3.sdk)

set(CMAKE_OSX_ARCHITECTURES "${CMAKE_OSX_ARCHITECTURES}" CACHE STRING "osx architectures")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" CACHE STRING "c++ flags")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "c flags")
set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}" CACHE PATH "osx sysroot")


#-isysroot /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.3.sdk

set(CMAKE_FIND_ROOT_PATH /Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator4.3.sdk)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
