set(tests
  #TestDrawPlane
  TestDrawPlaneGlut
  )

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})

macro(ves_add_test name)
  #set(deps ves GLESv2 EGL X11)
  set(deps ves ${GLUT_LIBRARIES} ${OPENGL_LIBRARIES})
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${deps})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name} ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
endmacro()

foreach(name ${tests})
  ves_add_test(${name})
endforeach()
