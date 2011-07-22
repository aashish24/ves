cmake_minimum_required(VERSION 2.8.4)

include(ExternalProject)

set(base "${CMAKE_BINARY_DIR}/CMakeExternals")
set_property(DIRECTORY PROPERTY EP_BASE ${base})

set(build_type Release)

set(source_prefix ${base}/Source)
set(build_prefix ${base}/Build)
set(install_prefix ${base}/Install)

set(module_defaults
  -DModule_CommonComputationalGeometry:BOOL=OFF
  -DModule_CommonCore:BOOL=OFF
  -DModule_CommonDataModel:BOOL=OFF
  -DModule_CommonMath:BOOL=OFF
  -DModule_CommonMisc:BOOL=OFF
  -DModule_CommonSystem:BOOL=OFF
  -DModule_CommonTransforms:BOOL=OFF
  -DModule_FiltersCutter:BOOL=ON
  -DModule_FiltersParallelStatistics:BOOL=OFF
  -DModule_FiltersStatistics:BOOL=OFF
  -DModule_IOCore:BOOL=OFF
  -DModule_IOGeometry:BOOL=ON
  -DModule_IOMySQL:BOOL=OFF
  -DModule_IOODBC:BOOL=OFF
  -DModule_IOPostgreSQL:BOOL=OFF
  -DModule_IOSQL:BOOL=OFF
  -DModule_IOXML:BOOL=ON
  -DModule_ParallelCore:BOOL=OFF
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


macro(crosscompile_ves proj toolchain_file)
  ExternalProject_Add(
    ${proj}
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/../../../src/ves
    DOWNLOAD_COMMAND ""
    INSTALL_COMMAND ""
    CMAKE_ARGS
      -DCMAKE_INSTALL_PREFIX:PATH=${install_prefix}/${proj}
      -DCMAKE_BUILD_TYPE:STRING=${build_type}
      -DCMAKE_TOOLCHAIN_FILE:FILEPATH=${CMAKE_SOURCE_DIR}/cmake/${toolchain_file}
  )

  ExternalProject_Add_Step(${proj} forcebuild
    COMMAND ${CMAKE_COMMAND} -E remove ${base}/Stamp/${proj}/${proj}-build
    DEPENDEES configure
    DEPENDERS build
    ALWAYS 1
  )

endmacro()

crosscompile_ves(ves-simulator toolchain-ios-simulator.cmake)
crosscompile_ves(ves-device toolchain-ios-device.cmake)
