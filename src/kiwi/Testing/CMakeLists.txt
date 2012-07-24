set(tests
  TestCap
  TestClipPlane
  TestGradientBackground
  TestKiwiViewer
  TestPointCloud
  TestTexture
  TestTexturedBackground
  TestWireframe
  )


macro(ves_add_test name)
  set(deps kiwi GLESv2 EGL X11)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${deps})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name} ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
endmacro()


foreach(name ${tests})
  ves_add_test(${name})
endforeach()
