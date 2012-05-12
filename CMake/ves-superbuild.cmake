if(NOT VES_LINUX_SUPERBUILD AND NOT VES_ANDROID_SUPERBUILD AND NOT VES_IOS_SUPERBUILD)
  return()
endif()

project(VES_SUPERBUILD NONE)

include(ExternalProject)


set(base "${CMAKE_BINARY_DIR}/CMakeExternals")
set_property(DIRECTORY PROPERTY EP_BASE ${base})

# set a default build type if it is undefined, then make sure it goes in the cache
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "Build configuration type" FORCE)
set(build_type ${CMAKE_BUILD_TYPE})

set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

set(toolchain_dir "${CMAKE_CURRENT_SOURCE_DIR}/CMake/toolchains")
set(ves_src "${CMAKE_CURRENT_SOURCE_DIR}")

find_package(PythonInterp REQUIRED)
find_package(Git REQUIRED)

set(module_defaults
  -DVTK_Group_StandAlone:BOOL=OFF
  -DVTK_Group_Rendering:BOOL=OFF
  -DModule_vtkFiltersCore:BOOL=ON
  -DModule_vtkFiltersModeling:BOOL=ON
  -DModule_vtkFiltersSources:BOOL=ON
  -DModule_vtkFiltersGeometry:BOOL=ON
  -DModule_vtkIOGeometry:BOOL=ON
  -DModule_vtkIOXML:BOOL=ON
  -DModule_vtkIOImage:BOOL=ON
  -DModule_vtkIOPLY:BOOL=ON
  -DModule_vtkImagingCore:BOOL=ON
  -DModule_vtkParallelCore:BOOL=ON
  -DModule_vtkRenderingCore:BOOL=ON
  -DModule_vtkRenderingFreeType:BOOL=ON
)

macro(force_build proj)
  ExternalProject_Add_Step(${proj} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove ${base}/Stamp/${proj}/${proj}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
  )
endmacro()

macro(install_eigen)
  set(eigen_url http://www.vtk.org/files/support/eigen-3.1.0-alpha1.tar.gz)
  set(eigen_md5 c04dedf4ae97b055b6dd2aaa01daf5e9)
  ExternalProject_Add(
    eigen
    SOURCE_DIR ${source_prefix}/eigen
    URL ${eigen_url}
    URL_MD5 ${eigen_md5}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory "${source_prefix}/eigen/Eigen" "${install_prefix}/eigen/Eigen"
  )
endmacro()

macro(compile_vtk proj)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${source_prefix}/vtk
    GIT_REPOSITORY git://github.com/patmarion/VTK.git
    GIT_TAG origin/kiwi-fixes
    INSTALL_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DBUILD_TESTING:BOOL=OFF
      ${module_defaults}
  )
endmacro()

macro(compile_ves proj)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${ves_src}
    DOWNLOAD_COMMAND ""
    DEPENDS vtk-host eigen
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DBUILD_TESTING:BOOL=ON
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DVES_USE_VTK:BOOL=ON
      -DVTK_DIR:PATH=${build_prefix}/vtk-host
      -DEIGEN_INCLUDE_DIR:PATH=${install_prefix}/eigen
      -DCMAKE_CXX_FLAGS:STRING=${VES_CXX_FLAGS}
  )

  force_build(${proj})
endmacro()

macro(crosscompile_vtk proj toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${base}/Source/vtk
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS vtk-host
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DBUILD_TESTING:BOOL=OFF
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${toolchain_dir}/${toolchain_file}
      -DVTKCompileTools_DIR:PATH=${build_prefix}/vtk-host
      ${module_defaults}
      -C ${toolchain_dir}/TryRunResults.cmake
  )
endmacro()

macro(crosscompile_ves proj tag toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${ves_src}
    DOWNLOAD_COMMAND ""
    DEPENDS vtk-${tag} eigen
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${toolchain_dir}/${toolchain_file}
      -DCMAKE_CXX_FLAGS:STRING=${VES_CXX_FLAGS}
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DVES_USE_VTK:BOOL=ON
      -DVTK_DIR:PATH=${build_prefix}/vtk-${tag}
      -DEIGEN_INCLUDE_DIR:PATH=${install_prefix}/eigen
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
  )

  force_build(${proj})
endmacro()

install_eigen()
compile_vtk(vtk-host)

if(VES_IOS_SUPERBUILD)
  crosscompile_vtk(vtk-ios-simulator toolchain-ios-simulator.cmake)
  crosscompile_vtk(vtk-ios-device toolchain-ios-device.cmake)
  crosscompile_ves(ves-ios-simulator ios-simulator toolchain-ios-simulator.cmake)
  crosscompile_ves(ves-ios-device ios-device toolchain-ios-device.cmake)
endif()

if(VES_ANDROID_SUPERBUILD)
  crosscompile_vtk(vtk-android android.toolchain.cmake)
  crosscompile_ves(ves-android android android.toolchain.cmake)
endif()

if(VES_LINUX_SUPERBUILD)
  compile_ves(ves-host)
endif()

set(ves_superbuild_enabled ON)


# CTestCustom.cmake needs to be placed at the top level build directory
configure_file(${CMAKE_SOURCE_DIR}/CMake/CTestCustom.cmake.in
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake COPYONLY)
