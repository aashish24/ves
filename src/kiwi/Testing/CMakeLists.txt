if ("${CMAKE_CXX_PLATFORM_ID}" STREQUAL "QNX")
  set(tests
    TestGradientBackgroundQNX
    TestKiwiViewerQNX
    TestTexturedBackgroundQNX
    TestTextureQNX
    TestWireframeQNX
    TestImageClipQNX
  )

  set(deps GLES_CM gf)
else()
  set(tests
    TestCap # ClipPlanes are not supported in ES 1.0
    TestClipPlane # ClipPlanes are not supported in ES 1.0
    TestGradientBackground
    TestKiwiViewer
  #  TestPointCloud
    TestTexture
    TestTexturedBackground
    TestWireframe
  )

  set(deps GLESv1_CM X11)
endif()

macro(ves_add_test name)
  set(deps ${deps} EGL ves kiwi)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${deps})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name} ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
endmacro()


foreach(name ${tests})
  ves_add_test(${name})
endforeach()
