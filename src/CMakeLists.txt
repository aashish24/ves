
# find and include eigen
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})

include(setup-headers.cmake)

add_subdirectory(ves)
add_subdirectory(shaders)

if(VES_USE_VTK)
  add_subdirectory(kiwi)
endif()
