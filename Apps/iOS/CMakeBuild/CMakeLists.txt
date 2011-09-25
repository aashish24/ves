cmake_minimum_required(VERSION 2.8.3)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}/CMakeExternals")
set_property(DIRECTORY PROPERTY EP_BASE ${base})

set(build_type Release)

set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

set(module_defaults
  -DModule_vtkFiltersCore:BOOL=ON
  -DModule_vtkFiltersSources:BOOL=ON
  -DModule_vtkIOGeometry:BOOL=ON
  -DModule_vtkIOXML:BOOL=ON
  -DModule_vtkIOImage:BOOL=ON
  -DModule_vtkImagingCore:BOOL=ON
)


set(proj vtkmodular-host)
ExternalProject_Add(
  ${proj}
  SOURCE_DIR ${source_prefix}/vtkmodular
  GIT_REPOSITORY git://gitorious.org/~patmarion/kitware/patmarion-vtkmodular.git
  GIT_TAG origin/kiwi-new
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DBUILD_SHARED_LIBS:BOOL=OFF
    ${module_defaults}
)


macro(crosscompile_vtk proj toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${base}/Source/vtkmodular
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND ""
    DEPENDS vtkmodular-host
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${toolchain_file}
      -DVTKCompileTools_DIR:PATH=${build_prefix}/vtkmodular-host
      -DBUILD_SHARED_LIBS:BOOL=OFF
      ${module_defaults}
      -C ${CMAKE_CURRENT_SOURCE_DIR}/cmake/TryRunResults.cmake
  )
endmacro()

crosscompile_vtk(vtkmodular-simulator toolchain-ios-simulator.cmake)
crosscompile_vtk(vtkmodular-device toolchain-ios-device.cmake)


macro(crosscompile_ves proj tag toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../..
    DOWNLOAD_COMMAND ""
    DEPENDS vtkmodular-${tag}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_CURRENT_SOURCE_DIR}/cmake/${toolchain_file}
      -DVES_USE_VTK:BOOL=ON
      -DVTK_DIR:PATH=${build_prefix}/vtkmodular-${tag}
  )

  ExternalProject_Add_Step(${proj} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove ${base}/Stamp/${proj}/${proj}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
  )

endmacro()

crosscompile_ves(ves-simulator simulator toolchain-ios-simulator.cmake)
crosscompile_ves(ves-device device toolchain-ios-device.cmake)
