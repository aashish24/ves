cmake_minimum_required(VERSION 2.8.3)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}/CMakeExternals")
set_property(DIRECTORY PROPERTY EP_BASE ${base})

set(build_type Debug)

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

set(proj vtkmodular)
ExternalProject_Add(
  ${proj}
  SOURCE_DIR ${source_prefix}/vtkmodular
  GIT_REPOSITORY git://gitorious.org/~patmarion/kitware/patmarion-vtkmodular.git
  GIT_TAG origin/kiwi-new
  INSTALL_COMMAND ""
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    ${module_defaults}
)


set(proj ves)
ExternalProject_Add(
  ${proj}
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../..
  DOWNLOAD_COMMAND ""
  DEPENDS vtkmodular
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    -DVES_USE_VTK:BOOL=ON
    -DVTK_DIR:PATH=${build_prefix}/vtkmodular
    -DBUILD_TESTING:BOOL=ON
)

ExternalProject_Add_Step(${proj} forcebuild
  COMMAND ${CMAKE_COMMAND} -E remove ${base}/Stamp/${proj}/${proj}-build
  DEPENDEES configure
  DEPENDERS build
  ALWAYS 1
)

