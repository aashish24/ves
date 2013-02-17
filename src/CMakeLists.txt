
# find and include eigen
find_package(Eigen REQUIRED)
include_directories(${EIGEN_INCLUDE_DIRS})

include(setup-headers.cmake)

# Configure VES
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/vesConfigure.h.in
  ${VES_BINARY_DIR}/vesConfigure.h @ONLY)

add_subdirectory(ves)

# TODO Commented out for now
if(VES_USE_VTK)
  add_subdirectory(kiwi)
endif()
