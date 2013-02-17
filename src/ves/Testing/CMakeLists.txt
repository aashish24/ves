set(tests
  TestDrawLine
  TestDrawPlane
  TestMatrix
)

if ("${CMAKE_CXX_PLATFORM_ID}" STREQUAL "QNX")
  set(deps GLES_CM gf)
else()
  set(deps GLESv1_CM X11)
endif()

macro(ves_add_test name)
  set(deps ves ${deps} EGL)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} ${deps})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name} ${VES_SOURCE_DIR} ${VES_SOURCE_DIR}/Testing/BaselineImages)
endmacro()

foreach(name ${tests})
  ves_add_test(${name})
endforeach()
