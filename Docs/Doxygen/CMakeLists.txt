project(doxygen)

find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
  message(STATUS "Doxygen not found - API documentation will not be built.")
  return()
endif()

configure_file(${doxygen_SOURCE_DIR}/Doxyfile.in ${doxygen_BINARY_DIR}/Doxyfile)

add_custom_target(docs-doxygen
  COMMAND ${CMAKE_COMMAND} -E remove_directory ${doxygen_BINARY_DIR}/html
  COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_BINARY_DIR}/Doxyfile
  )

