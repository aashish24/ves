set(tests
  TestDrawPlane
  TestMatrix
  )

find_package(GLUT REQUIRED)
include_directories(${GLUT_INCLUDE_DIRS})

macro(ves_add_test name)
  set(deps ves ${GLUT_glut_LIBRARY})
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${deps})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name} ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
endmacro()

foreach(name ${tests})
  ves_add_test(${name})
endforeach()
