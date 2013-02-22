
set(build_target "ios-device")

if(NOT DEFINED PCL_SUPERBUILD_DIR)
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



include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(sources
  vtkPCDReader.cxx
  vtkPCLConversions.cxx
  vtkPCLSACSegmentationPlane.cxx
  vtkPCLVoxelGrid.cxx
  )

set(deps
  ${PCL_SEGMENTATION_LIBRARY}
  ${PCL_SAMPLE_CONSENSUS_LIBRARY}
  ${PCL_FILTERS_LIBRARY}
  ${PCL_KDTREE_LIBRARY}
  ${PCL_IO_LIBRARY}
  ${PCL_COMMON_LIBRARY}
  ${BOOST_LIBRARIES}
  )

ves_add_library(vtkPCL "${sources}" "${deps}")
