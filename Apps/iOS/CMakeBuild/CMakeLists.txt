cmake_minimum_required(VERSION 2.8.4)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}/CMakeExternals")
set_property(DIRECTORY PROPERTY EP_BASE ${base})

set(build_type Release)

set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

set(module_defaults
  -DModule_vtkCommonComputationalGeometry:BOOL=OFF
  -DModule_vtkCommonCore:BOOL=OFF
  -DModule_vtkCommonDataModel:BOOL=OFF
  -DModule_vtkCommonMath:BOOL=OFF
  -DModule_vtkCommonMisc:BOOL=OFF
  -DModule_vtkCommonSystem:BOOL=OFF
  -DModule_vtkCommonTransforms:BOOL=OFF
  -DModule_vtkFiltersCutter:BOOL=ON
  -DModule_vtkFiltersParallelStatistics:BOOL=OFF
  -DModule_vtkFiltersStatistics:BOOL=OFF
  -DModule_vtkIOCore:BOOL=OFF
  -DModule_vtkIOGeometry:BOOL=ON
  -DModule_vtkIOMySQL:BOOL=OFF
  -DModule_vtkIOODBC:BOOL=OFF
  -DModule_vtkIOPostgreSQL:BOOL=OFF
  -DModule_vtkIOSQL:BOOL=OFF
  -DModule_vtkIOXML:BOOL=ON
  -DModule_vtkParallelCore:BOOL=OFF
)


set(proj vtkmodular-host)
ExternalProject_Add(
  ${proj}
  SOURCE_DIR ${source_prefix}/vtkmodular
  GIT_REPOSITORY git://gitorious.org/~patmarion/kitware/patmarion-vtkmodular.git
  GIT_TAG origin/kiwi
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
    -DCMAKE_BUILD_TYPE:STRING=${build_type}
    ${module_defaults}
)


macro(crosscompile proj toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${base}/Source/vtkmodular
    DOWNLOAD_COMMAND ""
    DEPENDS vtkmodular-host
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_SOURCE_DIR}/cmake/${toolchain_file}
      -DVTKCompileTools_DIR:PATH=${build_prefix}/vtkmodular-host
      ${module_defaults}
      -C ${CMAKE_SOURCE_DIR}/cmake/TryRunResults.cmake
  )
endmacro()

crosscompile(vtkmodular-simulator toolchain-ios-simulator.cmake)
crosscompile(vtkmodular-device toolchain-ios-device.cmake)


macro(crosscompile_ves proj tag toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../..
    DOWNLOAD_COMMAND ""
    DEPENDS vtkmodular-${tag}
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_SOURCE_DIR}/cmake/${toolchain_file}
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
