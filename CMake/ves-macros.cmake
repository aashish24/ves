
#
# Usage:
# set(files foo.h bar/bar.h)
# set(dir include/fooheaders)
# ves_install_files("${headers}" ${headers_location})
#
function(ves_install_files files dir)
  foreach(f ${files})
    get_filename_component(file_dir "${f}" PATH)
    install(FILES "${f}" DESTINATION "${dir}/${file_dir}")
  endforeach()
endfunction()

#
# Usage:
# set(files foo.h bar/bar.h)
# set(dir include/fooheaders)
# ves_install_files("${headers}" ${headers_location})
#
function(ves_install_build_files files dir)
  foreach(f ${files})
    get_filename_component(file_dir "${f}" PATH)
    file(RELATIVE_PATH file_rel_dir "${CMAKE_CURRENT_BINARY_DIR}" "${file_dir}")
    install(FILES "${f}" DESTINATION "${dir}/${file_rel_dir}")
  endforeach()
endfunction()

#
# Usage:
# set(sources foo.cpp bar.cpp)
# set(deps zlib)
# ves_add_library(foo "${sources}" "${deps}")
#
function(ves_add_library library_name sources deps)
  add_library(${library_name} ${sources})
  target_link_libraries(${library_name} ${deps})
  set_property(GLOBAL APPEND PROPERTY VES_LIBRARIES ${library_name})
  install(TARGETS ${library_name}
    EXPORT ${VES_TARGETS_NAME}
    RUNTIME DESTINATION ${VES_INSTALL_BIN_DIR}
    LIBRARY DESTINATION ${VES_INSTALL_LIB_DIR}
    ARCHIVE DESTINATION ${VES_INSTALL_LIB_DIR}
    PUBLIC_HEADER DESTINATION ${VES_INSTALL_INCLUDE_DIR})
endfunction()

#
#
#
function(ves_add_relative_include_dirs dirs)
  include_directories(${dirs})
  set_property(GLOBAL APPEND PROPERTY VES_RELATIVE_INCLUDE_DIRS ${dirs})
endfunction()

#
#
#
function(ves_add_build_relative_include_dirs dirs)
  foreach(dir ${dirs})
    include_directories("${CMAKE_CURRENT_BINARY_DIR}/${dir}")
  endforeach()
  set_property(GLOBAL APPEND PROPERTY VES_RELATIVE_BUILD_INCLUDE_DIRS ${dirs})
endfunction()
