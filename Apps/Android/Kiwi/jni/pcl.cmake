if (NOT USE_PCL)
  return()
endif()

set(build_target "android")

if(NOT DEFINED PCL_SUPERBUILD_DIR)
  set(PCL_SUPERBUILD_DIR "PCL_SUPERBUILD_DIR-NOTFOUND" CACHE FILEPATH "pcl mobile superbuild directory")
  message(FATAL_ERROR "Please set the cmake variable PCL_SUPERBUILD_DIR")
endif()

set(pcl_deps_install_dir ${PCL_SUPERBUILD_DIR}/CMakeExternals/Install)
if(NOT IS_DIRECTORY ${pcl_deps_install_dir})
  message(FATAL_ERROR "The directory ${pcl_deps_install_dir} does not exist.  Check the value of PCL_SUPERBUILD_DIR.")
endif()

list(APPEND CMAKE_FIND_ROOT_PATH
  ${pcl_deps_install_dir}/boost-${build_target}
  ${pcl_deps_install_dir}/flann-${build_target}
  ${pcl_deps_install_dir}/pcl-${build_target})

set(PCL_DIR ${pcl_deps_install_dir}/pcl-${build_target}/share/pcl-1.6)

find_package(PCL REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
