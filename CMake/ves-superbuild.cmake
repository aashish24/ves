project(VES_SUPERBUILD NONE)

include(ExternalProject)

option(VES_HOST_SUPERBUILD "Build VES and dependent subprojects for host architecture" OFF)
option(VES_ANDROID_SUPERBUILD "Build VES and dependent subprojects for Android" OFF)
option(VES_IOS_SUPERBUILD "Build VES and dependent subprojects for iOS" OFF)

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
  -DModule_vtkIOLegacy:BOOL=ON
  -DModule_vtkIOXML:BOOL=ON
  -DModule_vtkIOImage:BOOL=ON
  -DModule_vtkIOPLY:BOOL=ON
  -DModule_vtkIOInfovis:BOOL=ON
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
  set(eigen_url http://vtk.org/files/support/eigen-3.1.2.tar.gz)
  set(eigen_md5 bb639388192cb80f1ee797f5dbdbe74f)
  ExternalProject_Add(
    eigen
    SOURCE_DIR ${source_prefix}/eigen
    URL ${eigen_url}
    URL_MD5 ${eigen_md5}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory "${source_prefix}/eigen/Eigen" "${install_prefix}/eigen/Eigen"
                 && ${CMAKE_COMMAND} -E copy_directory "${source_prefix}/eigen/unsupported" "${install_prefix}/eigen/unsupported"
  )
endmacro()

macro(compile_vtk proj)
  if(NOT VES_HOST_SUPERBUILD)
    set(makecmd make)
    if(CMAKE_GENERATOR MATCHES "NMake Makefiles")
      set(makecmd nmake)
    endif()
    set(vtk_host_build_command BUILD_COMMAND ${makecmd} vtkCompileTools)
  endif()
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${source_prefix}/vtk
    GIT_REPOSITORY git://github.com/patmarion/VTK.git
    GIT_TAG ce4a267
    INSTALL_COMMAND ""
    ${vtk_host_build_command}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DBUILD_SHARED_LIBS:BOOL=ON
      -DBUILD_TESTING:BOOL=OFF
      ${module_defaults}
  )
endmacro()


macro(crosscompile_vtk proj toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${base}/Source/vtk
    DOWNLOAD_COMMAND ""
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


macro(compile_ves proj)
  set(tag host)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${ves_src}
    DOWNLOAD_COMMAND ""
    DEPENDS vtk-${tag} eigen
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DBUILD_TESTING:BOOL=ON
      -DBUILD_SHARED_LIBS:BOOL=OFF
      -DVES_USE_VTK:BOOL=ON
      -DVES_NO_SUPERBUILD:BOOL=ON
      -DVES_USE_DESKTOP_GL:BOOL=ON
      -DVTK_DIR:PATH=${build_prefix}/vtk-${tag}
      -DEIGEN_INCLUDE_DIR:PATH=${install_prefix}/eigen
      -DCMAKE_CXX_FLAGS:STRING=${VES_CXX_FLAGS} -fPIC
  )

  force_build(${proj})
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
      -DVES_NO_SUPERBUILD:BOOL=ON
      -DVTK_DIR:PATH=${build_prefix}/vtk-${tag}
      -DEIGEN_INCLUDE_DIR:PATH=${install_prefix}/eigen
      -DPYTHON_EXECUTABLE:FILEPATH=${PYTHON_EXECUTABLE}
  )

  force_build(${proj})
endmacro()

install_eigen()
compile_vtk(vtk-host)

if(VES_IOS_SUPERBUILD)
  foreach(target ios-simulator ios-device)
    crosscompile_vtk(vtk-${target} toolchain-${target}.cmake)
    crosscompile_ves(ves-${target} ${target} toolchain-${target}.cmake)
  endforeach()
endif()

if(VES_ANDROID_SUPERBUILD)
  crosscompile_vtk(vtk-android android.toolchain.cmake)
  crosscompile_ves(ves-android android android.toolchain.cmake)
endif()

if(VES_HOST_SUPERBUILD)
  compile_ves(ves-host)
endif()

# create frameworks for OSX
if(VES_IOS_SUPERBUILD)
    add_custom_target(kiwi-framework ALL
      COMMAND ${CMAKE_SOURCE_DIR}/CMake/makeFramework.sh kiwi
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS ves-ios-device ves-ios-simulator
      COMMENT "Creating kiwi framework")

    add_custom_target(vtk-framework ALL
      COMMAND ${CMAKE_SOURCE_DIR}/CMake/makeFramework.sh vtk
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS vtk-ios-device vtk-ios-simulator
      COMMENT "Creating vtk framework")
endif()


# CTestCustom.cmake needs to be placed at the top level build directory
configure_file(${CMAKE_SOURCE_DIR}/CMake/CTestCustom.cmake.in
               ${CMAKE_BINARY_DIR}/CTestCustom.cmake COPYONLY)
