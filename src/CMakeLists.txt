
# find and include eigen
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})

if(VES_USE_DESKTOP_GL)
  find_package(OpenGL REQUIRED)
  include_directories(${OPENGL_INCLUDE_DIRS})
  add_definitions(-DVES_USE_DESKTOP_GL)
  set(VES_OPENGL_LIBRARIES ${OPENGL_LIBRARIES})
endif()

include(setup-headers.cmake)

add_subdirectory(ves)
add_subdirectory(shaders)

if(VES_USE_VTK)
  add_subdirectory(kiwi)
endif()
