# Hack-
# VES removes Apple specific code when the ANDROID symbol is defined, which is normally defined
# by the android toolchain file.  We can get VES to build on Linux by defining ANDROID.
if(NOT APPLE)
  add_definitions(-DANDROID)
endif()

# find and include eigen
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})

include(setup-headers.cmake)

add_subdirectory(ves)
add_subdirectory(shaders)

if(VES_USE_VTK)
  add_subdirectory(vtkVES)
endif()
